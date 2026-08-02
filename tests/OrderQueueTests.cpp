// OrderQueueTests.cpp

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
  queue.Push({"Amoxicillin", 30});
  EXPECT_FALSE(queue.IsEmpty());
  EXPECT_EQ(queue.Size(), 1u);
}

TEST(OrderQueueTest, PopReturnsOrdersInFifoOrder) {
  OrderQueue queue;
  queue.Push({"Amoxicillin", 30});
  queue.Push({"Tylenol", 90});

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
  queue.Push({"Aspirin", 100});
  queue.Push({"Ibuprofen", 50});

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

TEST(OrderQueueTest, PushBatchIncreasesSizeByBatchCount) {
  OrderQueue queue;
  queue.PushBatch({{"Aspirin", 100}, {"Ibuprofen", 50}, {"Tylenol", 90}});
  EXPECT_EQ(queue.Size(), 3);
}

TEST(OrderQueueTest, PushBatchAssignsSequentialIds) {
  OrderQueue queue;
  queue.PushBatch({{"Aspirin", 100}, {"Ibuprofen", 50}, {"Tylenol", 90}});
  ASSERT_EQ(queue.Size(), 3);
  Order ord1, ord2, ord3;
  queue.Pop(ord1);
  queue.Pop(ord2);
  queue.Pop(ord3);
  EXPECT_EQ(ord1.id, 1);
  EXPECT_EQ(ord2.id, 2);
  EXPECT_EQ(ord3.id, 3);
}

TEST(OrderQueueTest, PushBatchContinuesIdSequence) {
  OrderQueue queue;
  queue.Push({"Amoxicillin", 150});
  queue.PushBatch({{"Aspirin", 100}, {"Ibuprofen", 50}, {"Tylenol", 90}});
  ASSERT_EQ(queue.Size(), 4);
  Order ord1, ord2, ord3, ord4;
  queue.Pop(ord1);
  queue.Pop(ord2);
  queue.Pop(ord3);
  queue.Pop(ord4);
  EXPECT_EQ(ord1.id, 1);
  EXPECT_EQ(ord1.drugName, "Amoxicillin");
  EXPECT_EQ(ord2.id, 2);
  EXPECT_EQ(ord2.drugName, "Aspirin");
  EXPECT_EQ(ord3.id, 3);
  EXPECT_EQ(ord3.drugName, "Ibuprofen");
  EXPECT_EQ(ord4.id, 4);
  EXPECT_EQ(ord4.drugName, "Tylenol");
}

TEST(OrderQueueTest, CalculateDurationZeroGuard) {
  OrderQueue queue;
  queue.Push({"Placebo", 0});
  Order ord1;
  queue.Pop(ord1);
  ASSERT_EQ(ord1.duration, 0);
}
