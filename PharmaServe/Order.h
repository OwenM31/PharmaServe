#pragma once
#include <string>

struct Order {
  unsigned int id;
  std::string drugName;
  unsigned int quantity;
  double duration;
};
