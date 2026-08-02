// OrderQueue.cpp

#include "OrderQueue.h"
#include "OrderRequest.h"
#include <cmath>
#include <cstdint>
#include <mutex>
#include <vector>

uint64_t OrderQueue::Push(const OrderRequest &request) {
  // Adds an order and wakes a `WorkerPop` thread.
  // Returns the assigned id.

  Order stampedOrder{};
  stampedOrder.drugName = request.drugName;
  stampedOrder.quantity = request.quantity;
  stampedOrder.duration = CalculateDuration(request);
  // ^ Build most of the order before locking.

  std::lock_guard<std::mutex> lock(mutex);
  stampedOrder.id = nextId++; // This needs to happen while the lock is held.

  orderQueue.push(stampedOrder);

  cv.notify_one();

  return stampedOrder.id;
}

std::vector<uint64_t>
OrderQueue::PushBatch(const std::vector<OrderRequest> &requestBatch) {
  std::vector<uint64_t> batchIds;
  // Returns vector of the assigned ids.

  std::lock_guard<std::mutex> lock(mutex); // Hold lock for the whole batch.
  for (auto req : requestBatch) {

    Order stampedOrder{};
    stampedOrder.id = nextId++;
    stampedOrder.drugName = req.drugName;
    stampedOrder.quantity = req.quantity;
    stampedOrder.duration = CalculateDuration(req);

    orderQueue.push(stampedOrder);
    batchIds.push_back(stampedOrder.id);
  }
  cv.notify_all();

  return batchIds;
}

bool OrderQueue::Pop(Order &outOrder) {
  // Safe to call from main thread.
  std::lock_guard<std::mutex> lock(mutex);
  if (orderQueue.empty()) {
    return false;
  }
  outOrder = orderQueue.front();
  orderQueue.pop();

  if (orderQueue.empty()) { // If now empty, alert `WaitUntilEmpty` thread.
    cv.notify_all();
  }

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

  if (orderQueue.empty()) { // If now empty, alert `WaitUntilEmpty` thread.
    cv.notify_all();
  }

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

  std::lock_guard lock(mutex);
  shuttingDown = true;

  cv.notify_all();
}

double OrderQueue::CalculateDuration(const OrderRequest &request) {
  if (request.quantity == 0)
    return 0;

  auto baseTime = 1;
  auto scale = 3; // log(100)=2, log(200)~=2.3, log(300)~=2.5

  return baseTime + scale * std::log(request.quantity);
}
