// WorkerHandle.h

#pragma once
#include <atomic>
#include <thread>

#include "WorkerStatus.h"

struct WorkerHandle {
  std::thread worker;
  std::atomic<bool> active; // When this is false, the worker finishes the
                            //     current order and stops.
  std::atomic<bool> finished{
      false}; // This is set to true when the last order
              //     finishes, then the thread is rejoined.
  WorkerStatus status;
};
