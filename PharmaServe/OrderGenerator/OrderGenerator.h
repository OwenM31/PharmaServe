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

  explicit OrderGenerator(OrderQueue &queue);
  ~OrderGenerator();

  State GetState();
  void Start();
  void Pause();
  void Resume();
  void Stop();

  void SetDelayRange(double min, double max);
  void SetQuantityRange(uint32_t min, uint32_t max);

  uint64_t SubmitRandomOrder();
  std::vector<uint64_t> SubmitRandomBatch(uint32_t batchSize);
  uint64_t SubmitOrder(OrderRequest req);
  std::vector<uint64_t> SubmitBatch(std::vector<OrderRequest> reqBatch);

private:
  std::atomic<State> state{State::PAUSED};
  OrderQueue &queue;
  std::thread generatorThread;

  double minDelay{1};
  double maxDelay{5};
  uint32_t minQuantity{3};
  uint32_t maxQuantity{15};

  std::mt19937 rng;
  std::mutex rngMutex;
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

  void RunLoop();
  OrderRequest BuildRandomRequest();
};
