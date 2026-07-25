// WorkerStatusTests.cpp

#include <gtest/gtest.h>
#include <optional>

#include "../PharmaServe/WorkerStatus.h"

TEST(WorkerStatusTest, StatusDefaultsToWaiting) {
  WorkerStatus status;
  EXPECT_EQ(status.GetState(), WorkerStatus::State::WAITING);
}

TEST(WorkerStatusTest, SetWorkingChangesSnapshot) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  auto snapshot{status.GetSnapshot()};
  EXPECT_EQ(snapshot.state, WorkerStatus::State::WORKING);
  EXPECT_EQ(snapshot.orderId, 1);
  EXPECT_EQ(snapshot.timeRemaining, 5.0);
}

TEST(WorkerStatusTest, SetWaitingChangesSnapshot) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  auto snapshot{status.GetSnapshot()};
  ASSERT_EQ(snapshot.state, WorkerStatus::State::WORKING);
  ASSERT_EQ(snapshot.orderId, 1);
  ASSERT_EQ(snapshot.timeRemaining, 5.0);
  status.SetWaiting();
  snapshot = status.GetSnapshot();
  EXPECT_EQ(snapshot.state, WorkerStatus::State::WAITING);
  EXPECT_EQ(snapshot.orderId, std::nullopt);
  EXPECT_EQ(snapshot.timeRemaining, std::nullopt);
}

TEST(WorkerStatusTest, SetShuttingDownChangesSnapshot) {
  WorkerStatus status;
  status.SetShuttingDown();
  auto snapshot{status.GetSnapshot()};
  EXPECT_EQ(snapshot.state, WorkerStatus::State::SHUTTING_DOWN);
}

TEST(WorkerStatusTest, SetShuttingDownKeepsInfo) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  auto snapshot{status.GetSnapshot()};
  ASSERT_EQ(snapshot.state, WorkerStatus::State::WORKING);
  ASSERT_EQ(snapshot.orderId, 1);
  ASSERT_EQ(snapshot.timeRemaining, 5.0);
  status.SetShuttingDown();
  snapshot = status.GetSnapshot();
  EXPECT_EQ(snapshot.state, WorkerStatus::State::SHUTTING_DOWN);
  EXPECT_EQ(snapshot.orderId, 1);
  EXPECT_EQ(snapshot.timeRemaining, 5.0);
}
