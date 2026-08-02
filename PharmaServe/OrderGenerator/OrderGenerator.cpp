// OrderGenerator.cpp

#include "OrderGenerator.h"
#include <random>

OrderGenerator::OrderGenerator(OrderQueue &orderQueue)
    : queue{orderQueue}, rng(std::random_device{}()) {}

OrderGenerator::~OrderGenerator() {
  state = State::STOPPED;
  if (generatorThread.joinable()) {
    generatorThread.join();
  }
}
