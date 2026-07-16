#include <gtest/gtest.h>
#include <thread>
#include <chrono>

#include "../PharmaServe/OrderQueue.h"

TEST(OrderQueueTest, StartsEmpty) {
	OrderQueue queue;
	EXPECT_TRUE(queue.IsEmpty());
	EXPECT_EQ(queue.Size(), 0u);
}

TEST(OrderQueueTest, PushIncreasesSize) {
	OrderQueue queue;
	queue.Push({1, "Amoxicillin", 30});
	EXPECT_FALSE(queue.IsEmpty());
	EXPECT_EQ(queue.Size(), 1u);
}

TEST(OrderQueueTest, PopReturnsOrdersInFifoOrder) {
	OrderQueue queue;
	queue.Push({1, "Amoxicillin", 30});
	queue.Push({2, "Tylenol", 90});

	Order first;
	ASSERT_TRUE(queue.WorkerPop(first));
	EXPECT_EQ(first.orderId, 1);

	Order second;
	ASSERT_TRUE(queue.WorkerPop(second));
	EXPECT_EQ(second.orderId, 2);
}

TEST(OrderQueueTest, PopFailsOnEmptyQueue) {
	OrderQueue queue;
	Order order;
	EXPECT_FALSE(queue.Pop(order));
}

TEST(OrderQueueTest, ShutdownUnblocksWaitingPop) {
	OrderQueue queue;
	bool popResult = false;

	std::thread worker([&queue, &popResult]() {
		Order order;
		queue.WorkerPop(order);
		popResult = true;
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	EXPECT_FALSE(popResult);

	queue.Shutdown();
	worker.join();

	EXPECT_TRUE(popResult);
}