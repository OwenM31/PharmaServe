#include "OrderQueue.h"

void OrderQueue::Push(const Order& order) {
	orderQueue.push(order);
}

bool OrderQueue::Pop(const Order& order) {
	if (!orderQueue.empty() && orderQueue.front().orderId == order.orderId) {
		orderQueue.pop();
		return true;
	}
	return false;
}

size_t OrderQueue::Size() const {
	return orderQueue.size();
}

bool OrderQueue::IsEmpty() const {
	return orderQueue.empty();
}

