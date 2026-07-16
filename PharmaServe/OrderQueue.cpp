#include "OrderQueue.h"

void OrderQueue::Push(const Order& order) {
	orderQueue.push(order);
}

bool OrderQueue::Pop(Order& outOrder) {
// Pop the front order into `outOrder` parameter
	if (orderQueue.empty()) {
		return false;
	}
	outOrder = orderQueue.front();
	orderQueue.pop();
	return true;
}

size_t OrderQueue::Size() const {
	return orderQueue.size();
}

bool OrderQueue::IsEmpty() const {
	return orderQueue.empty();
}

