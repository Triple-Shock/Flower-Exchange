#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>

constexpr auto OUTFILE = "execution_rep.csv";

class Order
{
private:
    void helper(int, double);

public:
    std::string order_id;
    std::string client_order_id;
    std::string instrument;
    int side;
    int exec_status;
    int quantity;
    double price;
    std::string reason;
    std::string transactionTime;

    Order(int, std::string, std::string, int, int, int, double);

    void write_csv();
    void write_csv(int);
    void write_csv(double);
    void write_csv(int, double);

    bool is_valid();
    std::string transaction_time();
};

void printOrders(const std::vector<Order>& orders);