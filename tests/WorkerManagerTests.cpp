// WorkerManagerTests.cpp

#include "../PharmaServe/OrderQueue/OrderQueue.h"
#include "../PharmaServe/WorkerManager/WorkerManager.h"
#include <cassert>
#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <thread>

class WorkerManagerTestFixture : public ::testing::Test {
protected:
  OrderQueue queue;
  std::unique_ptr<WorkerManager> manager;

  void SetUp() override { manager = std::make_unique<WorkerManager>(queue, 3); }
};
//
TEST_F(WorkerManagerTestFixture, ManagerSpawnsUpToMax) {
  auto id1{manager->SpawnWorker()};
  auto id2{manager->SpawnWorker()};
  auto id3{manager->SpawnWorker()};
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(id2, 2);
  EXPECT_EQ(id3, 3);
  EXPECT_THROW(manager->SpawnWorker(), std::runtime_error);
}

TEST_F(WorkerManagerTestFixture, StopWorkerChecksById) {
  auto id1{manager->SpawnWorker()};
  EXPECT_FALSE(manager->StopWorker(5u));
  EXPECT_TRUE(manager->StopWorker(id1));
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  auto snapshot{manager->GetStatus(id1)};
  EXPECT_TRUE(snapshot.has_value());
  EXPECT_EQ(snapshot->state, WorkerStatus::State::SHUTTING_DOWN);
}

TEST_F(WorkerManagerTestFixture, StopAllStopsAllWorkers) {
  auto id1{manager->SpawnWorker()};
  auto id2{manager->SpawnWorker()};
  manager->StopAll();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  EXPECT_EQ(manager->GetStatus(id1), std::nullopt);
  EXPECT_EQ(manager->GetStatus(id2), std::nullopt);
}

TEST_F(WorkerManagerTestFixture, GetStatusChecksById) {
  auto id1{manager->SpawnWorker()};
  auto snapshot1{manager->GetStatus(id1)};
  EXPECT_TRUE(snapshot1.has_value());
  EXPECT_EQ(snapshot1->state, WorkerStatus::State::WAITING);
  auto snapshot2{manager->GetStatus(5u)};
  EXPECT_FALSE(snapshot2.has_value());
  EXPECT_EQ(snapshot2, std::nullopt);
}

TEST_F(WorkerManagerTestFixture, GetAllStatusesMapIsComplete) {
  auto id1{manager->SpawnWorker()};
  auto map{manager->GetAllStatuses()};
  EXPECT_EQ(map.size(), 1);
  EXPECT_NE(map.find(id1), map.end());
  auto id2{manager->SpawnWorker()};
  map = manager->GetAllStatuses();
  EXPECT_EQ(map.size(), 2);
  EXPECT_NE(map.find(id2), map.end());
}

TEST_F(WorkerManagerTestFixture, ThreadJoinerJoinsStoppedThreads) {
  auto id1{manager->SpawnWorker()};
  auto map{manager->GetAllStatuses()};
  ASSERT_EQ(map.size(), 1);
  ASSERT_TRUE(manager->StopWorker(id1));
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  map = manager->GetAllStatuses();
  EXPECT_EQ(map.find(id1), map.end());
  EXPECT_EQ(map.size(), 0);
}

TEST_F(WorkerManagerTestFixture, SpawnWorkerAfterThreadJoinerStopsWorker) {
  auto id1{manager->SpawnWorker()};
  auto id2{manager->SpawnWorker()};
  auto id3{manager->SpawnWorker()};
  ASSERT_EQ(id1, 1);
  ASSERT_EQ(id2, 2);
  ASSERT_EQ(id3, 3);
  auto map{manager->GetAllStatuses()};
  ASSERT_EQ(map.size(), 3);
  ASSERT_THROW(manager->SpawnWorker(), std::runtime_error);
  manager->StopAll();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  map = manager->GetAllStatuses();
  EXPECT_EQ(map.size(), 0);
  auto id4{manager->SpawnWorker()};
  EXPECT_EQ(id4, 4);
}
