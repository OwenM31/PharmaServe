
#include <gtest/gtest.h>
#include <optional>

#include "../PharmaServe/WorkerManager.h"

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
