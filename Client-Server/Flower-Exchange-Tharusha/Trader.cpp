#pragma once
#include "Trader.h"

std::vector<Order> Trader::readOrders(const std::string& csvString)
{

	std::vector<Order> orders;

	// Create a stringstream from the CSV string
	std::istringstream csvStream(csvString);

	std::string line;
	std::getline(csvStream, line); // Skip the header line
	std::getline(csvStream, line); // Skip the header line 2
	int i = 1; // keep order count
	while (std::getline(csvStream, line)) {
		Order order = parseOrder(line, i);
		orders.push_back(order);
		i++;
	}

	return orders;
}

Order Trader::parseOrder(const std::string& line, int i) {

	std::istringstream iss(line);
	std::string field;

	std::string Client_Order_Id;
	std::string Instrument;
	int Side;
	int Quantity;
	double Price;
	std::string tempString;
	int Exec_status = 0;

	getline(iss, Client_Order_Id, ',');
	getline(iss, Instrument, ',');

	getline(iss, tempString, ',');
	Side = atoi(tempString.c_str());

	getline(iss, tempString, ',');
	Quantity = atoi(tempString.c_str());

	getline(iss, tempString, ',');
	Price = stold(tempString);

	Order order(i, Client_Order_Id, Instrument, Side, Exec_status, Quantity, Price);

	return order;
}
