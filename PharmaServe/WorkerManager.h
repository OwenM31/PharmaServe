// WorkerManager.h

// Needs to : - track multiple workers by id
// 			  - be able to stop them / check their status

// map<int, WorkerHandle> id-to-worker hashmap
// size_t worker-id counter

#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "OrderQueue.h"
#include "WorkerHandle.h"
#include "WorkerStatus.h"

class WorkerManager {
public:
  explicit WorkerManager(OrderQueue &queue);
  ~WorkerManager(); // Needs destructor for handling stray workers

  std::optional<int> SpawnWorker();
  bool StopWorker(int id);
  void StopAll();

  std::map<int, WorkerStatus::Snapshot> GetAllStatuses() const;
  WorkerStatus::Snapshot GetStatus(int id) const;

private:
  OrderQueue &queue;
  std::map<int, std::unique_ptr<WorkerHandle>> workers;
  int nextId{0};
  mutable std::mutex workersMutex;
}
