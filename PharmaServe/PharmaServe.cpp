#include "OrderQueue.h"
#include <chrono>
#include <iostream>
#include <thread>

void Worker(OrderQueue &orderQueue) {
  Order order;
  while (orderQueue.WorkerPop(order)) {
    std::cout << "[worker] dispensing order " << order.orderId << " ("
              << order.drugName << " x" << order.quantity << ")\n";
    // Wait for .4s to simulate the robot working.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[worker] completed order " << order.orderId << ".\n";
    std::cout << "Pending Orders: " << orderQueue.Size() << std::endl;
  }
  std::cout << "[worker] shutting down...\n";
}

int main() {
  std::cout << "=== Initializing PharmaServe order fulfillment system ==="
            << std::endl;

  OrderQueue orderQueue;

  std::thread worker(Worker, std::ref(orderQueue));

  std::cout << "\n[Ingesting Orders from source...]" << std::endl;
  orderQueue.Push({1, "Aspirin", 100});
  orderQueue.Push({2, "Ibuprofen", 200});
  orderQueue.Push({3, "Tylenol", 150});
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  orderQueue.Push({4, "Amoxicillin", 30});

  std::this_thread::sleep_for(std::chrono::seconds(3));
  orderQueue.Shutdown();
  worker.join();

  return 0;
}
