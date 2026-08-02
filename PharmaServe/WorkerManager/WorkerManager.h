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

#include "../OrderQueue/OrderQueue.h"
#include "WorkerHandle.h"
#include "WorkerStatus.h"

class WorkerManager {
public:
  WorkerManager(OrderQueue &queue, uint32_t maxWorkers);
  ~WorkerManager(); // Needs destructor for handling stray workers

  uint32_t SpawnWorker();
  void StopAll();
  bool StopWorker(uint32_t id);

  std::map<uint32_t, WorkerStatus::Snapshot> GetAllStatuses() const;
  std::optional<WorkerStatus::Snapshot> GetStatus(uint32_t id) const;

private:
  OrderQueue &queue;
  uint32_t maxWorkers;
  std::map<uint32_t, std::unique_ptr<WorkerHandle>> workers;
  uint32_t nextId{1};
  mutable std::shared_mutex workersMutex;

  std::thread threadJoiner;
  std::atomic<bool> threadJoinerRunning;
};
