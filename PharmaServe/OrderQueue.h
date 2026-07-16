#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

#include "Order.h"

class OrderQueue {
    private: 
        mutable std::mutex mutex;
        std:: condition_variable cv;
        std::queue<Order> orderQueue;
        bool shuttingDown = false;


    public:
        void Push(const Order& order);
    
        bool Pop(Order& outOrder);

        bool TryPop(Order& outOrder);
        
		size_t Size() const;
		bool IsEmpty() const;

        void Shutdown();

};