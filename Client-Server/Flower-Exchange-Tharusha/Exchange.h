#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "Order.h"

std::vector<std::string> get_instruments(const std::string& filename);
std::string getCurrentTimeFormatted();


class Exchange
{
private:
    std::mutex m_mutex_; // Mutex for thread safety
    std::vector<std::string> m_instrument_list = { "Rose", "Lavender", "Lotus", "Tulip", "Orchid" };

public:
    void processOrder(Order& order);
    bool isOrderValid(Order& order);
};

