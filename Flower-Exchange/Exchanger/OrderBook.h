#pragma once
#include "Order.h"

class OrderBook
{
    public:
        std::string instrument;
        std::vector<Order> buy;
        std::vector<Order> sell;
        void trade(Order & order);

        OrderBook(std::string);
        void print();

    private:
        static bool buy_compare(Order a, Order b);
        static bool sell_compare(Order a, Order b);
};
