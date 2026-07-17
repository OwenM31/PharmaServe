#include "OrderQueue.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>

std::mutex coutMutex;

void SimulateOrderStream(OrderQueue &orderQueue, int orderCount) {
  std::vector drugNames = {"Acetaminophen", "Amoxicillin",     "Aspirin",
                           "Ibuprofen",     "Pseudoephedrine", "Tylenol"};

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution drugDist(0, (int)drugNames.size());
  std::uniform_int_distribution quantityDist(1, 5);
  std::uniform_int_distribution delayDist(100, 500);

  std::cout << "\n[Generator] Starting simulation of " << orderCount
            << " orders...\n";

  for (int i = 1; i <= orderCount; i++) {
    int drugIndex{drugDist(rng)};
    int quantity{quantityDist(rng) * 50};
    orderQueue.Push({i, drugNames[drugIndex], quantity});
    {
      std::lock_guard lock(coutMutex);
      std::cout << "[Generator] Pushed order " << i << " ("
                << drugNames[drugIndex] << " x" << quantity << ")\n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayDist(rng)));
  }

  std::cout << "[Generator] All orders pushed. Waiting for worker to drain "
               "queue...\n";
  orderQueue.WaitUntilEmpty();
  std::cout << "[Generator] Queue is empty. Shutting down...\n";
  orderQueue.Shutdown();
}

void Worker(OrderQueue &orderQueue) {
  Order order;
  while (orderQueue.WorkerPop(order)) {
    {
      std::lock_guard lock(coutMutex);
      std::cout << "[worker] dispensing order " << order.orderId << " ("
                << order.drugName << " x" << order.quantity << ")\n";
    } // Wait for .4s to simulate the robot working.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    {
      std::lock_guard lock(coutMutex);
      std::cout << "[worker] completed order " << order.orderId << ".\n";
      std::cout << "Pending Orders: " << orderQueue.Size() << std::endl;
    }
  }
  std::cout << "[worker] shutting down...\n";
}

int main(int argc, char *argv[]) {
  std::cout << "=== Initializing PharmaServe order fulfillment system ==="
            << std::endl;

  OrderQueue orderQueue;

  bool simulateMode{false};
  int orderCount{10};

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--simulate") {
      simulateMode = true;
    } else if (arg == "--count" && i + 1 < argc) {
      orderCount = std::stoi(argv[i + 1]);
      i++;
    }
  }

  std::thread worker(Worker, std::ref(orderQueue));

  if (simulateMode) {
    std::thread generator(SimulateOrderStream, std::ref(orderQueue),
                          orderCount);
    generator.join();
  } else {
    std::cout << "\n[Ingesting Orders from source...]" << std::endl;
    orderQueue.Push({1, "Aspirin", 100});
    orderQueue.Push({2, "Ibuprofen", 200});
    orderQueue.Push({3, "Tylenol", 150});
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    orderQueue.Push({4, "Amoxicillin", 30});

    std::this_thread::sleep_for(std::chrono::seconds(3));
    orderQueue.Shutdown();
  }

  worker.join();

  return 0;
}
