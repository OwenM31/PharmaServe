// WorkerManager.cpp

#include "WorkerManager.h"

WorkerManager::WorkerManager(OrderQueue &queue, int maxWorkers)
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
