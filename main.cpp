#include <iostream> 
#include <unordered_map>
#include <map>
#include <cstdint>
#include <memory>
#include <list>
#include <format>
#include <vector>
#include <numeric>
#include <stdexcept>

enum class OrderType{
	Market, 
	Limit };

enum class Side{
	Buy,
	Sell
};


// Aliases/Nicknames 
using Price = uint32_t;
using OrderId = uint64_t;
using Quantity = uint32_t;
using TradeId = uint32_t;

class Order{

	private:
		Side side_;
		Price price_;
		OrderId orderId_;
		OrderType orderType_;
		Quantity initialQuantity_;
		Quantity remainingQuantity_;
	
	public:
		Order(const Side& side, const Price& price, const OrderId& orderId, const OrderType& orderType, const Quantity& initialQuantity, const Quantity& remainingQuantity)
			: side_ { side }
			, price_ { price }
			, orderId_ { orderId }
			, orderType_ { orderType } 
			, initialQuantity_ { initialQuantity } 
			, remainingQuantity_ { remainingQuantity } 
			{}

		const Side& getSide() const { return side_; }
		const Price& getPrice() const { return price_; } 
		const OrderId& getOrderId() const { return orderId_; } 
		const OrderType& getOrderType() const { return orderType_; }
	       	const Quantity& getInitialQuantity() const { return initialQuantity_; }
		const Quantity& getRemainingQuantity() const { return remainingQuantity_; } 
		Quantity getFilledQuantity() const { return initialQuantity_ - remainingQuantity_; } 
		bool isFilled() const { return getFilledQuantity() == 0; }
		void Fill(const Quantity& quantity){
			if(quantity > getRemainingQuantity()){
				throw std::logic_error(std::format("Quantity to fill is larger than the remaining quantity for Order({})", getOrderId()));
			}	
			
			remainingQuantity_ -= quantity;	

		}

			


};


class Trade{
	private:
		TradeId tradeId_;
		OrderId buyOrderId_;
		OrderId sellOrderId_;
		Quantity quantity_;
		Price price_;
	public:
		Trade(const TradeId& tradeId, const OrderId& buyOrderId, const OrderId& sellOrderId, const Quantity& quantity, const Price& price)
			: tradeId_ { tradeId }
			, buyOrderId_ { buyOrderId }
			, sellOrderId_ { sellOrderId } 
			, quantity_ { quantity }
			, price_ { price } 
			{}	

		const TradeId& getTradeId() const { return tradeId_; }
		const OrderId& getBuyOrderId() const { return buyOrderId_; } 
		const OrderId& getSellOrderId() const { return sellOrderId_; }  
		const Quantity& getQuantity() const { return quantity_; }
		const Price& getPricce() const { return price_; }


};

// Aliases/Nicknames
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;
using Trades = std::vector<Trade>;


class OrderBook{
	private:
		// ** Bids need to be in order from greatest to least representing the best bids ** //
		// ** Ask need to be in order from least to greatest representing the best asks ** //
		std::map<Price, OrderPointers, std::greater<Price>> bids_;
		std::map<Price, OrderPointers, std::less<Price>> asks_;
		struct OrderEntry{
				OrderPointer order_ { nullptr };
				OrderPointers::iterator location_;		
		};
		std::unordered_map<OrderId, OrderEntry> orders_;
	public:
		
		void matchOrder(const OrderPointer& incomingOrder){

			// First determine the side of the order 
			Side incomingOrderSide = incomingOrder->getSide();
			

			if(incomingOrderSide == Side::Buy){

				// Try to fill the market order if the order type is a market  
				if(incomingOrder->getOrderType() == OrderType::Market){
					Quantity quantityNeeded = incomingOrder->getRemainingQuantity();
					Quantity totalQuantity = 0;
					
					//** YOU NEED TO LOCK ASKS HERE **//	
					

					// Use an iterator to go through the map 
					auto it = asks_.begin();

					// Continue going through the entire map until you have either filled your entire quantity 
					// Or you reach the end in which you do nothing 

					
					//*** POTENTIALLY COULD BE BETTER OPTIMIZED BUT FOR NOW IT WILL STAY AS IS ***///
					while(it != asks_.end() && totalQuantity < quantityNeeded){

						// Get the minimum between what needs to be filled and how much you can fill 
						for(const auto& order: it->second){
							totalQuantity += std::min(quantityNeeded - totalQuantity, order->getRemainingQuantity());
						}

						++it;

					}
					
					// Not enough to fill the market order 
					if(totalQuantity < quantityNeeded){
						return;

					}

					// Complete the order by matching here
					// Make sure to consider not having to go back again through the map and then filling the order 
					// Find something more optimal 
				}


				// If order is unable to match with best sell then add to orderbook	
				if(*getBestAsk() > incomingOrder->getPrice()){
					
					// Gonna need to lock the map
					OrderPointers& orderList = bids_[incomingOrder->getPrice()];
					orderList.push_back(incomingOrder);

					const auto& it = std::prev(orderList.end());// Points to the actual last element and not the end cuz of prev
					
					// Maybe might also need to lock the unordered_map due to hash collisions? Not sure I think I don't 
					OrderEntry orderEntry;
					orderEntry.order_ = incomingOrder;
					orderEntry.location_ = it;	
					
					orders_[incomingOrder->getOrderId()] = orderEntry;
					// Only need to lock once i need it i believe
					return;
				}

				
				// Do matching logic here 




			}

					
			if(incomingOrderSide == Side::Sell){
				
				// Try to fill the market order if the order type is a market  
				if(incomingOrder->getOrderType() == OrderType::Market){
					Quantity quantityNeeded = incomingOrder->getRemainingQuantity();
					Quantity totalQuantity = 0;
					
					//** YOU NEED TO LOCK BIDS HERE **//	
					

					// Use an iterator to go through the map 
					auto it = bids_.begin();

					// Continue going through the entire map until you have either filled your entire quantity 
					// Or you reach the end in which you do nothing 

					
					//*** POTENTIALLY COULD BE BETTER OPTIMIZED BUT FOR NOW IT WILL STAY AS IS ***///
					while(it != bids_.end() && totalQuantity < quantityNeeded){

						// Get the minimum between what needs to be filled and how much you can fill 
						for(const auto& order: it->second){
							totalQuantity += std::min(quantityNeeded - totalQuantity, order->getRemainingQuantity());
						}

						++it;

					}
					
					// Not enough to fill the market order 
					if(totalQuantity < quantityNeeded){
						return;

					}

					// Complete the order by matching here
					// Make sure to consider not having to go back again through the map and then filling the order 
					// Find something more optimal 
				}

				// If order is unable to match with best sell then add to orderbook	
				if(*getBestBid() < incomingOrder->getPrice()){
					// add order to order book 

					// Gonna need to lock the map
					auto& orderList = asks_[incomingOrder->getPrice()];
					orderList.push_back(incomingOrder);

					const auto& it = std::prev(orderList.end());// Points to the actual last element and not the end cuz of prev
					
					// Maybe might also need to lock the unordered_map due to hash collisions? Not sure I think I don't 
					OrderEntry orderEntry;
					orderEntry.order_ = incomingOrder;
					orderEntry.location_ = it;	
					
					orders_[incomingOrder->getOrderId()] = orderEntry;
					// Only need to lock once i need it i believe
					return;
				}

				// Do matching logic here




			}

			// Then try to match with the best available offer on the other side i.e the first in the map
				// If you can't match then add to order book i.e the best offer is greater than the order's asking/buying price

			// If you can match you match the price based on the minimum between the order price and the best price 
			// Then you try to fill the order by going through the list of orders at the price 
				// Continue going through the unfilled orders until the order is filled 	
				// If unable to fill order fully add to the order book (Unless a market order fok, going to have to check before fill)
				//
			// Try to match the full order
				// If market order then math with the best available bid/ask
				// If market order cannot be fully filled FOK
				
			// Anything remaining insert into book (Only if limit order)	
					
					
		
		}
		
		void modifyOrder(const OrderId& orderId){}
		void cancelOrder(const OrderId& orderId){

			// Find the order if the order doesn't exist throw 
			const auto it = orders_.find(orderId);
			if(it == orders_.end()){
				throw std::runtime_error(std::format("Order ({}) doesn't exist", orderId));
			}


				
			// Do not allow others to access the order 
			// Cancel order 
			// Unlock 	

			
		}
		
		// Check to see if this can be inlined
		const Price* getBestBid() const { 	
			if(!bids_.empty()){ 
				return &bids_.begin()->first;
			}		
			return nullptr;
		}


		//Check to see if this can be inlined 
		//Will return possible address of the pointer
		const Price* getBestAsk() const { 
			if(!asks_.empty()){
				return &asks_.begin()->first;
			}	
		
			return nullptr;		
		}
};	
