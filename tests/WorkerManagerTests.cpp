
#include <gtest/gtest.h>
#include <optional>

#include "../PharmaServe/WorkerManager.h"

TEST(WorkerStatusTest, StatusDefaultsToWaiting) {
  WorkerStatus status;
  EXPECT_EQ(status.GetState(), "WAITING");
}

TEST(WorkerStatusTest, SetWorkingChangesSnapshot) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  auto snapshot{status.GetSnapshot()};
  EXPECT_EQ(snapshot.state, "WORKING");
  EXPECT_EQ(snapshot.orderId, 1);
  EXPECT_EQ(snapshot.timeRemaining, 5.0);
}

TEST(WorkerStatusTest, SetWaitingChangesSnapshot) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  ASSERT_EQ(snapshot.state, "WORKING");
  ASSERT_EQ(snapshot.orderId, 1);
  ASSERT_EQ(snapshot.timeRemaining, 5.0);
  status.SetWaiting();
  auto snapshot{status.GetSnapshot()};
  EXPECT_EQ(snapshot.state, "WAITING");
  EXPECT_EQ(snapshot.orderId, std::nullopt);
  EXPECT_EQ(snapshot.timeRemaining, std::nullopt);
}

TEST(WorkerStatusTest, SetShuttingDownChangesSnapshot) {
  WorkerStatus status;
  status.SetShuttingDown();
  auto snapshot{status.GetSnapshot()};
  EXPECT_EQ(snapshot.state, "SHUTTING_DOWN");
}

TEST(WorkerStatusTest, SetShuttingDownKeepsInfo) {
  WorkerStatus status;
  status.SetWorking(1, 5.0);
  auto snapshot{status.GetSnapshot()};
  ASSERT_EQ(snapshot.state, "WORKING");
  ASSERT_EQ(snapshot.orderId, 1);
  ASSERT_EQ(snapshot.timeRemaining, 5.0);
  status.SetShuttingDown();
  snapshot = status.GetSnapshot();
  EXPECT_EQ(snapshot.state, "SHUTTING_DOWN");
  EXPECT_EQ(snapshot.orderId, 1);
  EXPECT_EQ(snapshot.timeRemaining, 5.0);
}
