// WorkerManager.h

// Needs to : - track multiple workers by id
// 			  - be able to stop them / check their status

// map<int, WorkerHandle> id-to-worker hashmap
// size_t worker-id counter

#pragma once
#include <map>
#include <memory>
#include <optional>
#include <shared_mutex>

#include "OrderQueue.h"
#include "WorkerHandle.h"
#include "WorkerStatus.h"

class WorkerManager {
public:
  explicit WorkerManager(OrderQueue &queue, unsigned int maxWorkers);
  ~WorkerManager(); // Needs destructor for handling stray workers

  std::optional<unsigned int> SpawnWorker();
  bool StopWorker(unsigned int id);
  void StopAll();

  std::map<unsigned int, WorkerStatus::Snapshot> GetAllStatuses() const;
  WorkerStatus::Snapshot GetStatus(int id) const;

private:
  OrderQueue &queue;
  unsigned int maxWorkers;
  std::map<unsigned int, std::unique_ptr<WorkerHandle>> workers;
  unsigned int nextId{0};
  mutable std::shared_mutex workersMutex;
};
