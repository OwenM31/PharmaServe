#pragma once
#include "Order.h"
#include <queue>

class OrderQueue {
    private: 
        std::queue<Order> orderQueue;
    public:
        void Push(const Order& order);
    
        bool Pop(Order& order);
        
		size_t Size() const;

		bool IsEmpty() const;

};