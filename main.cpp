#include <iostream> 
#include <unordered_map>
#include <map>
#include <cstdint>
#include <memory>
#include <list>
#include <format>
#include <vector>
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
				throw std::logic_error("Quantity cannot be filled for remaining quantity because quantity is larger than remaining");	
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

	public:

		void matchOrder(const OrderId& orderId){
			// Try to match the full order
				// If market order then math with the best available bid/ask
				// If market order cannot be fully filled FOK
			
			// Anything remaining insert into book (Only if limit order)	
		
		
		
		}
		

		bool cancelOrder(const OrderId& orderId){

			// Find the order if the order doesn't exist throw an error saying order doesn't exist 
			

			// Do not allow others to access the order 
			// Cancel order 
			// Unlock 	

			return true;


		}
		
		// Check to see if this can be inlined
		const Price& getBestBid() const { 	
			if(!bids_.empty()){ 
				
				const auto& bestBid = bids_.begin()->first;	
				
				
				return bestBid;
			}		
	
			throw std::runtime_error("No bids available");
		}


		//Check to see if this can be inlined 
		const Price& getBestAsk() const { 
			if(!asks_.empty()){
				const auto& bestAsk = asks_.begin()->first;

				return bestAsk;

			}	
				
			throw std::runtime_error("No asks available");		
		}
};	
