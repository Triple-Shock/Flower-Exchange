#pragma once
#include "Order.h"

void printOrders(const std::vector<Order>& orders) {
    for (const auto& order : orders) {
        std::cout << "Client Order ID: " << order.clientOrderId << std::endl;
        std::cout << "Instrument: " << order.instrument << std::endl;
        std::cout << "Side: " << order.side << std::endl;
        std::cout << "Quantity: " << order.quantity << std::endl;
        std::cout << "Price: " << order.price << std::endl;
        std::cout << "Execution Status: " << order.executionStatus << std::endl;
        std::cout << "Reason: " << order.reason << std::endl;
        std::cout << "Transaction Time: " << order.transactionTime << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
    }
}

