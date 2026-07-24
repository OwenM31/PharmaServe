// WorkerManager.cpp

#include "WorkerManager.h"
#include "WorkerStatus.h"
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <thread>

WorkerManager::WorkerManager(OrderQueue &queue, unsigned int maxWorkers)
    : queue(queue), maxWorkers(maxWorkers), nextId(0),
      threadJoinerRunning(true) {
  threadJoiner = std::thread(
      [this]() { // This thread automatically joins workers when they finish.
        while (threadJoinerRunning.load()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(200));

          std::unique_lock<std::shared_mutex> lock(workersMutex);
          for (auto w = workers.begin(); w != workers.end();) {
            if (w->second->finished.load()) {
              if (w->second->worker.joinable()) {
                w->second->worker.join();
              }
              w = workers.erase(w);
            } else {
              w++;
            }
          }
        }
      });
}

WorkerManager::~WorkerManager() {
  StopAll();
  while (true) {
    {
      std::shared_lock<std::shared_mutex> lock(workersMutex);
      if (workers.empty()) {
        break;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  threadJoinerRunning.store(false);
  if (threadJoiner.joinable()) {
    threadJoiner.join();
  }
}

void WorkerManager::StopAll() { // Simply flip the atomic<bool> to false then
                                // let threadJoiner clean up.
  std::shared_lock<std::shared_mutex> lock(workersMutex);

  for (auto &[id, handle] : workers) {
    handle->active.store(false);
  }
}

bool WorkerManager::StopWorker(unsigned int id) {
  std::shared_lock<std::shared_mutex> lock(workersMutex);

  auto idx{workers.find(id)};
  if (idx == workers.end()) {
    return false;
  }
  idx->second->active.store(false);
  return true;
}

std::optional<unsigned int> WorkerManager::SpawnWorker() {
  std::unique_lock<std::shared_mutex> lock(workersMutex);

  if (workers.size() >= maxWorkers) {
    throw std::runtime_error("Cannot spawn worker: max workers reached");
  }

  unsigned int id{nextId++};

  auto handle{std::make_unique<WorkerHandle>()};
  handle->active.store(true);

  WorkerHandle *handlePtr{
      handle.get()};               // Copy raw address to use within the thread.
  workers[id] = std::move(handle); // Move ownership to the id map.

  handlePtr->worker = std::thread(
      [this, handlePtr]() { // Workers watch the queue and pick up orders.
        bool keepRunning{true};

        while (keepRunning) {
          if (!handlePtr->active.load()) {
            keepRunning = false;
          }

          Order order;
          bool gotOrder{keepRunning && queue.Pop(order)};

          if (gotOrder) {
            handlePtr->status.SetWorking(order.id, order.duration);
            double remaining{order.duration};
            const double tickSeconds{0.1};
            while (remaining > 0.0) {
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
              remaining -= tickSeconds;
              if (!handlePtr->active.load()) {
                handlePtr->status.SetShuttingDown();
              }
              if (remaining > 0.0 && handlePtr->active.load()) {
                handlePtr->status.SetWorking(order.id, remaining);
              }
            }
          } else if (keepRunning) {
            handlePtr->status.SetWaiting();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        }
        handlePtr->status.SetShuttingDown();
      });

  return id;
}

std::map<unsigned int, WorkerStatus::Snapshot>
WorkerManager::GetAllStatuses() const {
  std::shared_lock<std::shared_mutex> lock(workersMutex);

  std::map<unsigned int, WorkerStatus::Snapshot> result;
  for (const auto &[id, handle] : workers) {
    result[id] = handle->status.GetSnapshot();
  }
  return result;
}

std::optional<WorkerStatus::Snapshot>
WorkerManager::GetStatus(unsigned int id) const {
  std::shared_lock<std::shared_mutex> lock(workersMutex);

  auto idx{workers.find(id)};
  if (idx == workers.end()) {
    return std::nullopt;
  }
  return idx->second->status.GetSnapshot();
}
