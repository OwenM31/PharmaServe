// WorkerStatus.cpp

#include "WorkerStatus.h"

// Getters
WorkerStatus::Snapshot WorkerStatus::GetSnapshot() const {
  std::shared_lock lock(statusMutex);
  return Snapshot{state, currentOrderId, currentTimeRemaining};
}

WorkerStatus::State WorkerStatus::GetState() const {
  std::shared_lock lock(statusMutex);
  return state;
}

std::optional<uint32_t> WorkerStatus::GetCurrentOrderId() const {
  std::shared_lock lock(statusMutex);
  return currentOrderId;
}

std::optional<double> WorkerStatus::GetTimeRemaining() const {
  std::shared_lock lock(statusMutex);
  return currentTimeRemaining;
}

// Setters
void WorkerStatus::SetWaiting() {
  std::unique_lock lock(statusMutex);
  state = State::WAITING;
  currentOrderId = std::nullopt;
  currentTimeRemaining = std::nullopt;
}

void WorkerStatus::SetWorking(int orderId, double timeRemaining) {
  std::unique_lock lock(statusMutex);
  state = State::WORKING;
  currentOrderId = orderId;
  currentTimeRemaining = timeRemaining;
}

void WorkerStatus::SetShuttingDown() {
  std::unique_lock lock(statusMutex);
  state = State::SHUTTING_DOWN;
  currentOrderId = std::nullopt;
  currentTimeRemaining = std::nullopt;
}
