
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

#include "Order.h"
#include "Using.h" 
#include "OrderType.h"
#include "Side.h"
#include "Containers.h"

class OrderBook{
	private: 
        
		Trades trades_;
    	TradeId nextTradeId{1};

		// ** Bids need to be in order from greatest to least representing the best bids ** //
		// ** Ask need to be in order from least to greatest representing the best asks ** //
		std::map<Price, OrderPointers, std::greater<Price>> bids_;
		std::map<Price, OrderPointers, std::less<Price>> asks_;
		// https://stackoverflow.com/questions/78518484/seamlessly-using-maps-with-different-comparators

		Quantity quantityOfBids_{};
		Quantity quantityOfAsks_{};

		struct OrderEntry{
			OrderPointer order_ { nullptr };
			OrderPointers::iterator location_;		
		};

		std::unordered_map<OrderId, OrderEntry> orders_;

		template<typename OrderMap>	       
		void fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder);

		template<typename OrderMap>
		void addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder);

	public:

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
        
		void processOrder(const OrderPointer& incomingOrder);
		void modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price);

		void cancelOrder(const OrderId& orderId);
};	

#endif 