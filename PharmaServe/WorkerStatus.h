// WorkerStatus.h

// status: WORKING / WAITING / SHUTTING DOWN / CONNECTING
// which_order (optional)
// time_remaining (optional)
// 1 shared_mutex to protect the data,

#pragma once
#include <cstdint>
#include <optional>
#include <shared_mutex>

class WorkerStatus {

public:
  enum class State { CONNECTING, WAITING, WORKING, SHUTTING_DOWN };

  struct Snapshot {
    State state;
    std::optional<uint32_t> currentOrderId;
    std::optional<double> currentTimeRemaining;
  };

  // Helper Methods
  WorkerStatus::Snapshot GetSnapshot() const;
  WorkerStatus::State GetState() const;
  std::optional<uint32_t> GetCurrentOrderId() const;
  std::optional<double> GetTimeRemaining() const;

  void SetWaiting();
  void SetWorking(int orderId, double timeRemaining);
  void SetShuttingDown();

private:
  std::shared_mutex statusMutex;
  State state;
  std::optional<uint32_t> currentOrderId;
  std::optional<double> currentTimeRemaining;
};
