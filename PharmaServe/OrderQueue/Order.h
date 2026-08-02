// Order.h

#pragma once
#include <string>

struct Order {
  uint32_t id;
  std::string drugName;
  uint32_t quantity;
  double duration;
};
