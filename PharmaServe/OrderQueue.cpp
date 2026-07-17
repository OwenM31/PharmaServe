#include "OrderQueue.h"
#include <mutex>

void OrderQueue::Push(const Order &order) {
  // Adds an order and wakes a `WorkerPop` thread.
  {
    std::lock_guard<std::mutex> lock(mutex);
    orderQueue.push(order);
  }
  cv.notify_one();
}

bool OrderQueue::Pop(Order &outOrder) {
  // Safe to call from main thread.
  std::lock_guard<std::mutex> lock(mutex);
  if (orderQueue.empty()) {
    return false;
  }
  outOrder = orderQueue.front();
  orderQueue.pop();
  return true;
}

bool OrderQueue::WorkerPop(Order &outOrder) {
  // Wait to be notified about an order in the queue.
  std::unique_lock lock(mutex);
  cv.wait(lock, [this] { return !orderQueue.empty() || shuttingDown; });
  //	Only use this method within a worker thread.
  //	If called from the main thread, it will freeze if the queue is empty.

  if (orderQueue.empty()) {
    // Only gets here if `shuttingDown == true`.
    return false;
  }
  outOrder = orderQueue.front();
  orderQueue.pop();
  return true;
}

size_t OrderQueue::Size() const {
  std::lock_guard lock(mutex);
  return orderQueue.size();
}

bool OrderQueue::IsEmpty() const {
  std::lock_guard lock(mutex);
  return orderQueue.empty();
}

void OrderQueue::WaitUntilEmpty() {
  std::unique_lock lock(mutex);
  cv.wait(lock, [this]() { return orderQueue.empty(); });
}

void OrderQueue::Shutdown() {
  // Wakes all `WorkerPop` threads to shutdown.
  {
    std::lock_guard lock(mutex);
    shuttingDown = true;
  }
  cv.notify_all();
}
