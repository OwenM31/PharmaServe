// OrderGenerator.h

#pragma once
#include "../OrderQueue/OrderQueue.h"
#include <atomic>
#include <cstdint>
#include <random>
#include <string>
#include <thread>
#include <vector>

class OrderGenerator {

public:
  enum class State { RUNNING, PAUSED, STOPPED };

  OrderGenerator(OrderQueue &queue);
  ~OrderGenerator();

  State GetState();
  void Start();
  void Pause();
  void Resume();
  void Stop();

  void SetDelayRange(double min, double max);
  void SetQuantityRange(uint32_t min, uint32_t max);

  void SubmitRandomOrder();
  void SubmitRandomBatch(uint32_t batchSize);
  void SubmitOrder(OrderRequest req);
  void SubmitBatch(std::vector<OrderRequest> reqBatch);

private:
  std::atomic<State> state{State::PAUSED};
  OrderQueue &queue;
  std::thread generatorThread;
  double minDelay{1};
  double maxDelay{5};
  uint32_t minQuantity{30};
  uint32_t maxQuantity{150};

  std::mt19937 rng;
  std::vector<std::string> drugNames = {"Acetaminophen",
                                        "Albuterol",
                                        "Amlodipine",
                                        "Amoxicillin",
                                        "Aspirin",
                                        "Atorvastatin",
                                        "Azithromycin",
                                        "Cetirizine",
                                        "Diphenhydramine",
                                        "Doxycycline",
                                        "Duloxetine",
                                        "Escitalopram",
                                        "Fluoxetine",
                                        "Fluticasone",
                                        "Furosemide",
                                        "Gabapentin",
                                        "Hydrochlorothiazide",
                                        "Ibuprofen",
                                        "Levothyroxine",
                                        "Lisinopril",
                                        "Loratadine",
                                        "Losartan",
                                        "Metformin",
                                        "Metoprolol",
                                        "Montelukast",
                                        "Naproxen",
                                        "Omeprazole",
                                        "Prednisone",
                                        "Rosuvastatin",
                                        "Sertraline"};
};
