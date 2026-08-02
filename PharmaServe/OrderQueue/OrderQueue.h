// OrderQueue.h

#pragma once
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <vector>

#include "Order.h"
#include "OrderRequest.h"

class OrderQueue {
private:
  mutable std::mutex mutex;
  std::condition_variable cv;
  std::queue<Order> orderQueue;
  uint64_t nextId{1};
  bool shuttingDown = false;

  double CalculateDuration(const OrderRequest &request);

public:
  uint64_t Push(const OrderRequest &request);
  std::vector<uint64_t>
  PushBatch(const std::vector<OrderRequest> &requestBatch);

  bool WorkerPop(Order &outOrder);

  bool Pop(Order &outOrder);

  size_t Size() const;
  bool IsEmpty() const;

  void Shutdown();

  void WaitUntilEmpty();
};
