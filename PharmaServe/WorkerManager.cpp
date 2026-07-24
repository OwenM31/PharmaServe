// WorkerManager.cpp

#include "WorkerManager.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>

WorkerManager::WorkerManager(OrderQueue &queue, unsigned int maxWorkers)
    : queue(queue), maxWorkers(maxWorkers), nextId(0) {}

WorkerManager::~WorkerManager() { StopAll(); }

void WorkerManager::StopAll() {
  std::lock_guard lock(workersMutex);

  for (auto &[id, handle] : workers) {
    handle->active.store(false);
  }

  for (auto &[id, handle] : workers) {
    if (handle->worker.joinable()) {
      handle->worker.join();
    }
  }

  workers.clear();
}

std::optional<unsigned int> SpawnWorker() {
  std::unique_lock lock(workersMutex);

  if (workers.size() >= maxWorkers) {
    throw std::runtime_error("Cannot spawn worker: max workers reached");
  }

  unsigned int id{nextId++};

  auto handle{std::make_unique<WorkerHandle>()};
  handle->active.store(true);

  WorkerHandle *handlePtr{handle.get()}; // copying raw address
  workers[id] = std::move(handle);       // moving ownership

  handlePtr->worker = std::thread([this, handlePtr]() {
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
