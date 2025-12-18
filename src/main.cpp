
#include <fstream> 
#include <iostream> 
#include <string> 
#include <sstream>
#include <cctype>
#include <atomic> 
#include <thread>

#include "OrderBook.cpp"



// Chatted this but i understand it 
void processOrder(const std::string& orderString, OrderBook& orderBook) {
	std::istringstream iss(orderString);

	std::string sideStr, priceStr, orderIdStr, orderTypeStr, initialQtyStr, remainingQtyStr;
	iss >> sideStr >> priceStr >> orderIdStr >> orderTypeStr >> initialQtyStr >> remainingQtyStr;

	// Convert to proper types
	Side side = (sideStr == "BUY") ? Side::Buy : Side::Sell;
	uint32_t price = static_cast<uint32_t>(std::stoul(priceStr));
	uint64_t orderId = std::stoull(orderIdStr);
	OrderType orderType = (orderTypeStr == "LIMIT") ? OrderType::Limit: OrderType::Market;
	uint32_t initialQty = static_cast<Quantity>(std::stoul(initialQtyStr));
	uint32_t remainingQty = static_cast<Quantity>(std::stoul(remainingQtyStr));
	// Create unique_ptr<Order>
	auto orderPtr = std::make_shared<Order>(side, price, orderId, orderType, initialQty, remainingQty);
	orderBook.processOrder(orderPtr);
}

int main(){
	OrderBook orderBook{};
	uint64_t nextOrderId = 1;

	for(int i{}; i < 1'000'000'000; ++i){
		Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
		uint32_t price = 100 + (i %50);
		uint32_t qty = 10 + (i % 100);
		uint64_t orderId = nextOrderId++;


	auto order = std::make_shared<Order>(side, price, orderId, OrderType::Limit, qty, qty);

	orderBook.processOrder(order);

	}
	return 0;
}
