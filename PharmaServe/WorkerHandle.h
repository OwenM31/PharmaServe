// WorkerHandle.h

// 1 thread,
// 1 atomic<bool> to signal shutdown,
// 1 WorkerStatus to hold info,
// 1 shared_mutex to protect WorkerStatus,
