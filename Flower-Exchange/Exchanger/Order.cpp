#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Order.h"

void printOrders(const std::vector<Order>& orders) {
	for (const auto& order : orders) {
		std::cout << "Client Order ID: " << order.client_order_id << std::endl;
		std::cout << "Instrument: " << order.instrument << std::endl;
		std::cout << "Side: " << order.side << std::endl;
		std::cout << "Quantity: " << order.quantity << std::endl;
		std::cout << "Price: " << order.price << std::endl;
		std::cout << "Execution Status: " << order.exec_status << std::endl;
		std::cout << "Reason: " << order.reason << std::endl;
		std::cout << "Transaction Time: " << order.transactionTime << std::endl;
		std::cout << "-----------------------------------------" << std::endl;
	}
}

Order::Order(int i, std::string Client_Order_ID, std::string Instrument, int Side, int Exec_Status, int Quantity, double Price)
{
	order_id = "ord" + std::to_string(i);
	client_order_id = Client_Order_ID;
	instrument = Instrument;
	side = Side;
	exec_status = Exec_Status;
	quantity = Quantity;
	price = Price;
	reason = "";
}

std::string Order::transaction_time()
{
	auto now = std::chrono::system_clock::now();
	time_t currentTime = std::chrono::system_clock::to_time_t(now);
	tm* timeInfo = localtime(&currentTime);
	std::ostringstream oss;
	oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S");

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() % std::chrono::seconds(1));
	oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

	return oss.str();
}

void Order::helper(int quantity, double price)
{
	std::ofstream file(OUTFILE, std::ios::app);
	if (file.is_open())
		file << order_id << "," << client_order_id << "," << instrument << "," << side << "," << exec_status << "," << quantity << "," << std::fixed << std::setprecision(2) << price << "," << reason << "," << transaction_time() << std::endl;
	file.close();
}

void Order::write_csv()
{
	helper(quantity, price);
}

void Order::write_csv(int Quantity)
{
	helper(Quantity, price);
}

void Order::write_csv(double Price)
{
	helper(quantity, Price);
}

void Order::write_csv(int Quantity, double Price)
{
	helper(Quantity, Price);
}

bool Order::is_valid()
{
	if (client_order_id.empty())
	{
		exec_status = 1;
		reason = "Invalid Client Order ID";
		return false;
	}

	if (!(instrument == "Rose" || instrument == "Lavender" || instrument == "Lotus" || instrument == "Tulip" || instrument == "Orchid"))
	{
		exec_status = 1;
		reason = "Invalid instrument";
		return 0;
	}

	if (!(side == 1 || side == 2))
	{
		exec_status = 1;
		reason = "Invalid side";
		return false;
	}

	if (quantity < 10 || quantity > 1000 || quantity % 10 != 0)
	{
		exec_status = 1;
		reason = "Invalid size";
		return false;
	}

	if (price <= 0.00)
	{
		exec_status = 1;
		reason = "Invalid price";
		return false;
	}

	return true;
}
