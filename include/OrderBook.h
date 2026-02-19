
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
#include "OrderState.h"

class OrderBook{
	private: 
		// Preallocated mem
		size_t bufferSize_;
		std::unique_ptr<std::byte[]> rawMemory_;
		std::pmr::monotonic_buffer_resource pool_;

		// Containers
		Trades trades_;
		std::pmr::map<Price, OrderPointers, std::greater<Price>> bids_; // greatest to smallest
		std::pmr::map<Price, OrderPointers, std::less<Price>> asks_; // smallest to largest

		struct OrderEntry{
			OrderPointer order_ { nullptr };
			OrderPointers::iterator location_;		
		};
		std::pmr::unordered_map<OrderId, OrderEntry> orders_;

		struct OrderStatus
		{
			Price price {};
			OrderType type {};
			Side side {};
			OrderState state {OrderState::Processing};
			Quantity remainingQuantity {};
			Quantity filledQuantity {0};
		};
		std::pmr::unordered_map<OrderId, OrderStatus> statusCache_;

		// Numericals
		TradeId nextTradeId_; // For simplicity trade ids will start from 1 
		Quantity quantityOfBids_;
		Quantity quantityOfAsks_;

		// Custom Template Helpers 
		template<typename OrderMap>	       
		void fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder);

		template<typename OrderMap>
		void addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder);
	
	public:
		OrderBook()
		: bufferSize_ {1024 * 1024 * 1024} // 1 GB
		, rawMemory_{std::make_unique<std::byte[]>(bufferSize_)}
		, pool_{rawMemory_.get(), bufferSize_}
		, trades_(&pool_)
		, bids_{&pool_}
		, asks_{&pool_}
		, orders_{&pool_}
		, statusCache_{&pool_}
		, nextTradeId_{1}
		, quantityOfBids_{}
		, quantityOfAsks_{}
		{
			trades_.reserve(100'000);
			orders_.reserve(100'000);
		}
        
		[[nodiscard]] bool processOrder(Order order); // Later down the road have it return the order id 
		[[nodiscard]] bool modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price);
		[[nodiscard]] bool cancelOrder(const OrderId& orderId);
		[[nodiscard]] const OrderStatus reviewOrderStatus(const OrderId& orderId) const;
		
		[[nodiscard]] inline const Trades& getTrades() const noexcept {return trades_; }
		[[nodiscard]] inline const Quantity& getQuantityOfAsks() const noexcept { return quantityOfAsks_; }
		[[nodiscard]] inline const Quantity& getQuantityOfBids() const noexcept { return quantityOfBids_; } 	

		// Public functions 
		[[nodiscard]] const Price getBestBid() const { 	
			if(!bids_.empty()){ 
				return bids_.begin()->first;
			}		
			return 0;
		}

		[[nodiscard]] const Price getBestAsk() const { 
			if(!asks_.empty()){
				return asks_.begin()->first;
			}	
		
			return 0;		
		}

		void display(size_t depth = 5) const; // This method is unpractical but it helps to visualize so I'll keep it 

		// No Copying 
		OrderBook(const OrderBook& other) = delete;
		OrderBook& operator=(const OrderBook& other) = delete;
		
		// No Moving 
		OrderBook(OrderBook&& other) = delete;
		OrderBook& operator=(OrderBook&& other) = delete;

};	


#endif 

