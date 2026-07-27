#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

#include "Order.h"

class OrderQueue {
private:
  mutable std::mutex mutex;
  std::condition_variable cv;
  std::queue<Order> orderQueue;
  bool shuttingDown = false;

public:
  void Push(const Order &order);

  bool WorkerPop(Order &outOrder);

  bool Pop(Order &outOrder);

  size_t Size() const;
  bool IsEmpty() const;

  void Shutdown();

  void WaitUntilEmpty();
};
