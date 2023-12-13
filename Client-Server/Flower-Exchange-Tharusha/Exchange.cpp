#pragma once


#include "Trader.h"
#include "Exchange.h"
#include "Order.h"

// get a list of valid instruments
std::vector<std::string> get_instruments(const std::string& filename) {
	std::vector<std::string> instrument_list;
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return instrument_list; // Return an empty vector in case of an error
	}

	std::string line;
	std::getline(file, line); // Skip the header line

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string field;
		std::getline(iss, field, ',');
		instrument_list.push_back(field);
	}

	file.close();
	return instrument_list;
}

//std::string getCurrentTimeFormatted() {
//	// Get the current time point
//	auto now = std::chrono::system_clock::now();
//
//	// Convert the current time point to a time_t object
//	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
//
//	// Convert the time_t object to a tm structure
//	std::tm* timeInfo = std::localtime(&currentTime);
//
//	// Get milliseconds from the current time point
//	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
//
//	// Format the time as YYYYMMDD-HHMMSS.sss
//	std::ostringstream oss;
//	oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S") << '.' << std::setw(3) << std::setfill('0') << milliseconds.count();
//
//	return oss.str();
//}

void Exchange::processOrder(Order& order)
{
	// Implement order processing logic against the order book
	// Update order execution status
	// ...
	std::lock_guard<std::mutex> lock(m_mutex_);
	order.executionStatus = rejected;  // Placeholder, update accordingly
}

bool Exchange::isOrderValid(Order& order)
{
	if (!std::count(m_instrument_list.begin(), m_instrument_list.end(), order.instrument)) {
		order.executionStatus = rejected;
		order.reason = "Instrument is not supported";
		order.transactionTime = " ";
		return true;
	}
	else if (order.side != 1 && order.side != 2) {
		order.executionStatus = rejected;
		order.reason = "Invalid side";
		order.transactionTime = " ";
		return true;
	}
	else if (order.price < 0) {
		order.executionStatus = rejected;
		order.reason = "Invalid price";
		order.transactionTime = " ";
		return true;
	}
	else if (order.quantity < 10 || order.quantity > 1000) {
		order.executionStatus = rejected;
		order.reason = "Quantity is out of the range.";
		order.transactionTime = " ";
		return true;
	}
	else if (order.quantity % 10 != 0) {
		order.executionStatus = rejected;
		order.reason = "Quantity must be a multiple of 10.";
		order.transactionTime = " ";
		return true;
	}
	return false;


}