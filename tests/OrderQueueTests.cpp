#include <gtest/gtest.h>
#include <thread>

#include "../PharmaServe/OrderQueue/OrderQueue.h"

TEST(OrderQueueTest, StartsEmpty) {
  OrderQueue queue;
  EXPECT_TRUE(queue.IsEmpty());
  EXPECT_EQ(queue.Size(), 0u);
}

TEST(OrderQueueTest, PushIncreasesSize) {
  OrderQueue queue;
  queue.Push({1, "Amoxicillin", 30, 5.0});
  EXPECT_FALSE(queue.IsEmpty());
  EXPECT_EQ(queue.Size(), 1u);
}

TEST(OrderQueueTest, PopReturnsOrdersInFifoOrder) {
  OrderQueue queue;
  queue.Push({1, "Amoxicillin", 30, 5.0});
  queue.Push({2, "Tylenol", 90, 5.0});

  Order first;
  ASSERT_TRUE(queue.WorkerPop(first));
  EXPECT_EQ(first.id, 1);

  Order second;
  ASSERT_TRUE(queue.WorkerPop(second));
  EXPECT_EQ(second.id, 2);
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

TEST(OrderQueueTest, WaitUntilEmptyBlocksUntilQueueDrained) {
  OrderQueue queue;
  queue.Push({1, "Aspirin", 100, 5.0});
  queue.Push({2, "Ibuprofen", 50, 5.0});

  std::atomic<bool> waitFinished(false);
  std::thread waiting_thread([&queue, &waitFinished]() {
    queue.WaitUntilEmpty();
    waitFinished = true;
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_FALSE(waitFinished);

  Order order;
  ASSERT_TRUE(queue.Pop(order));
  ASSERT_TRUE(queue.Pop(order));

  waiting_thread.join();
  EXPECT_TRUE(waitFinished);
}
