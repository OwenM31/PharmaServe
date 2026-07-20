// WorkerHandle.h

// 1 thread,
// 1 atomic<bool> to signal shutdown,
// 1 WorkerStatus to hold info,

#pragma once
#include <atomic>
#include <thread>

#include "WorkerStatus.h"

struct WorkerHandle {
  std::thread worker;
  std::atomic<bool> active;
  WorkerStatus status;
};
