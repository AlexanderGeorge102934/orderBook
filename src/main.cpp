
#include <fstream> 
#include <iostream> 
#include <string> 
#include <sstream>
#include <cctype>
#include <atomic> 
#include <thread>

#include "OrderBook.cpp"

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
