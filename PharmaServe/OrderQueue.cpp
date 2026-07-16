#include "OrderQueue.h"

void OrderQueue::Push(const Order& order) {
// Adds an order and wakes a consumer
	{
	std::lock_guard<std::mutex> lock(mutex);
	orderQueue.push(order);
	}
	cv.notify_one();
}

bool OrderQueue::Pop(Order& outOrder) {
// Wait to be notified about an order in the queue.
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock, [this] { return !orderQueue.empty() || shuttingDown;});
//	Only use this method within a worker thread.
//	If called from the main thread, it will freeze if the queue is empty.

	if (orderQueue.empty()) {
		return false;
	}
	outOrder = orderQueue.front();
	orderQueue.pop();
	return true;
}

size_t OrderQueue::Size() const {
	std::lock_guard<std::mutex> lock(mutex);
	return orderQueue.size();
}

bool OrderQueue::IsEmpty() const {
	std::lock_guard<std::mutex> lock(mutex);
	return orderQueue.empty();
}

