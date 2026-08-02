// OrderGenerator.cpp

#include "OrderGenerator.h"
#include <chrono>
#include <cstdint>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

OrderGenerator::OrderGenerator(OrderQueue &orderQueue)
    : queue{orderQueue}, rng(std::random_device{}()) {}

OrderGenerator::~OrderGenerator() { Stop(); }

void OrderGenerator::Stop() {
  state = State::STOPPED;
  if (generatorThread.joinable()) {
    generatorThread.join();
  }
}

void OrderGenerator::Start() {
  state = State::RUNNING;
  generatorThread = std::thread(&OrderGenerator::RunLoop, this);
}

void OrderGenerator::RunLoop() {
  while (state != State::STOPPED) {
    std::uniform_real_distribution<double> delayDist(minDelay, maxDelay);
    double delay{};

    { // Just lock the rng mutex minimally.
      std::lock_guard<std::mutex> lock(rngMutex);
      delay = delayDist(rng);
    }
    std::this_thread::sleep_for(std::chrono::duration<double>(delay));

    if (state == State::RUNNING) {
      SubmitRandomOrder();
    }
  }
}

OrderRequest OrderGenerator::BuildRandomRequest() {
  std::lock_guard<std::mutex> lock(rngMutex);
  std::uniform_int_distribution<size_t> nameDist(0, drugNames.size() - 1);
  std::uniform_int_distribution<uint32_t> quantityDist(minQuantity,
                                                       maxQuantity);
  return OrderRequest{drugNames[nameDist(rng)], 10 * quantityDist(rng)};
}

uint64_t OrderGenerator::SubmitOrder(OrderRequest req) {
  return queue.Push(req);
}

std::vector<uint64_t>
OrderGenerator::SubmitBatch(std::vector<OrderRequest> reqBatch) {
  return queue.PushBatch(reqBatch);
}

uint64_t OrderGenerator::SubmitRandomOrder() {
  return SubmitOrder(BuildRandomRequest());
}

std::vector<uint64_t> OrderGenerator::SubmitRandomBatch(uint32_t batchSize) {
  std::vector<OrderRequest> requests;
  for (auto i{0}; i < batchSize; i++) {
    requests.push_back(BuildRandomRequest());
  }
  return SubmitBatch(requests);
}
