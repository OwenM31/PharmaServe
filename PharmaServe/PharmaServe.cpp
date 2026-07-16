#include <iostream>
#include "OrderQueue.h"




int main()
{
    std::cout << "=== Initializing PharmaServe order fulfillment system ===" << std::endl;

    OrderQueue orderQueue;

	std::cout << "\n[Ingesting Orders from source...]" << std::endl;
	orderQueue.Push({ 1, "Aspirin", 100 });
    orderQueue.Push({ 2, "Ibuprofen", 200 });
	orderQueue.Push({ 3, "Tylenol", 150 });

	std::cout << "Pending Orders: " << orderQueue.Size() << std::endl;
	Order currentOrder;
	while (orderQueue.Pop(currentOrder)) {
		std::cout << " -> Dispensing Order #" << currentOrder.orderId
			<< ": " << currentOrder.quantity << "x "
			<< currentOrder.drugName << std::endl;
	}
	std::cout << "\nQueue processing complete. Remaining: " << orderQueue.Size() << std::endl;

    return 0;
}

