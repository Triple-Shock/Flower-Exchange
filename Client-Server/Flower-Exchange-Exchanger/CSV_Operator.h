#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Order.h"
class CSV_Operator
{
	public:
	CSV_Operator();
	~CSV_Operator();
	std::vector<Order> readOrders(const std::string& inputFile);
	Order parseOrder(const std::string& line);
	void writeExecutionReport(const std::vector<Order>& orders, const std::string& outputFile);
};

