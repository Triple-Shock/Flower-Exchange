#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "Order.h"

// To process csv string and create orders vector
class Trader
{
public:
    Order parseOrder(const std::string& line, int i);
    std::vector<Order> readOrders(const std::string& inputFile);
};
