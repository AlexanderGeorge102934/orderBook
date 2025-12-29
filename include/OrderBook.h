
#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <string>  
#include <format>  
#include <atomic> 
#include <mutex>
#include <condition_variable>

#include "Order.h"
#include "Using.h" 
#include "OrderType.h"
#include "Side.h"
#include "Containers.h"

class OrderBook{
	private: 
		// Putting how much trades I expect to reserve space and reduce overhead of mem alloc
		// For simplicity it is hardcoded but I will create a constructor that
		// Calculates how much mem to alloc based on expected number of orders/hardware of computer
       		static constexpr size_t EXPECTED_ORDERS = 100'000'000;
	        static constexpr size_t EXPECTED_TRADES = 100'000'000;
		static constexpr size_t EXPECTED_ORDERS_PER_PRICE = 100;	

		Trades trades_;
    		TradeId nextTradeId; // For simplicity trade ids will start from 1 

		// ** Bids need to be in order from greatest to least representing the best bids ** //
		// ** Ask need to be in order from least to greatest representing the best asks ** //
		std::map<Price, OrderPointers, std::greater<Price>> bids_;
		std::map<Price, OrderPointers, std::less<Price>> asks_;
		// https://stackoverflow.com/questions/78518484/seamlessly-using-maps-with-different-comparators

		Quantity quantityOfBids_;
		Quantity quantityOfAsks_;

		struct OrderEntry{
			OrderPointer order_ { nullptr };
			OrderPointers::iterator location_;		
		};

		std::unordered_map<OrderId, OrderEntry> orders_;

		mutable std::mutex mut_;
		std::condition_variable dataCondition_;

		// Custom Template Helpers 
		template<typename OrderMap>	       
		void fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder);

		template<typename OrderMap>
		void addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder);

		// Simple Getters
		[[nodiscard]] const Price* getBestBid() const { 	
			if(!bids_.empty()){ 
				return &bids_.begin()->first;
			}		
			return nullptr;
		}

		[[nodiscard]] const Price* getBestAsk() const { 
			if(!asks_.empty()){
				return &asks_.begin()->first;
			}	
		
			return nullptr;		
		}

		[[nodiscard]] inline const Quantity& getQuantityOfAsks() const noexcept { return quantityOfAsks_; }
		[[nodiscard]] inline const Quantity& getQuantityOfBids() const noexcept { return quantityOfBids_; } 	
		[[nodiscard]] inline const Trades& getTrades() const noexcept {return trades_; }


	public:
		OrderBook()
		: trades_{}
		, nextTradeId{1}
		, bids_{}
		, asks_{}
		, quantityOfBids_{}
		, quantityOfAsks_{}
		, orders_{}
		, mut_{}
		, dataCondition_{}
		{
			trades_.reserve(EXPECTED_TRADES);
			orders_.reserve(EXPECTED_ORDERS);
		}
        
		void processOrder(const OrderPointer& incomingOrder);
		void modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price);

		void cancelOrder(const OrderId& orderId);

		// Deleted Constructors

		// No Copying 
		OrderBook(const OrderBook& other) = delete;
		OrderBook& operator=(const OrderBook& other) = delete;
		
		// No Moving 
		OrderBook(OrderBook&& other) = delete;
		OrderBook& operator=(OrderBook&& other) = delete;
};	


#endif 
