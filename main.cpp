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
#include <limits>
#include <atomic> 

enum class OrderType{
	Market, 
	Limit 
};

enum class Side{
	Buy,
	Sell
};

using Price = uint32_t;
using OrderId = uint64_t;
using Quantity = uint32_t;
using TradeId = uint32_t;

namespace tradingValueConstants{

	constexpr Price INVALID_PRICE = std::numeric_limits<Price>::max();

}

namespace IdGenerator{
    std::atomic<OrderId> nextOrderId = 1;
    std::atomic<TradeId> nextTradeId = 1;

    OrderId generateOrderId(){
        return nextOrderId++;
    
    }

    TradeId generateTradeId(){
        return nextTradeId++;
    }
}

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
			{
                // The quantity should never be less than 1 when initialized otherwise the order is filled             
				if (initialQuantity < 1) {
                    throw std::invalid_argument(std::format("Order({}) must have initial quantity >= 1 (got {})", orderId, initialQuantity));
                }
                if (remainingQuantity < 1) {
                    throw std::invalid_argument(std::format("Order({}) must have remaining quantity >= 1 (got {})", orderId, remainingQuantity));
                }
                if(remainingQuantity_ > initialQuantity_){
                    throw std::invalid_argument(std::format("Order({}) cannot have a greater remaining quantity than initial", orderId));
                }

            }

		[[nodiscard]] const Side& getSide() const noexcept { return side_; }
		[[nodiscard]] const Price& getPrice() const noexcept { return price_; } 
		[[nodiscard]] const OrderId& getOrderId() const noexcept { return orderId_; } 
		[[nodiscard]] const OrderType& getOrderType() const noexcept { return orderType_; }
	    [[nodiscard]] const Quantity& getInitialQuantity() const noexcept { return initialQuantity_; }
		[[nodiscard]] const Quantity& getRemainingQuantity() const noexcept { return remainingQuantity_; } 
		[[nodiscard]] Quantity getFilledQuantity() const noexcept { return initialQuantity_ - remainingQuantity_; } 
		[[nodiscard]] bool isFilled() const noexcept { return getFilledQuantity() == 0; }
		void Fill(const Quantity& quantity){

			// Assumption is that the quantity if greater should be handled before passing the quantity as an argument
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

		[[nodiscard]] const TradeId& getTradeId() const noexcept { return tradeId_; }
		[[nodiscard]] const OrderId& getBuyOrderId() const noexcept { return buyOrderId_; } 
		[[nodiscard]] const OrderId& getSellOrderId() const noexcept { return sellOrderId_; }  
		[[nodiscard]] const Quantity& getQuantity() const noexcept { return quantity_; }
		[[nodiscard]] const Price& getPrice() const noexcept { return price_; }


};

// Aliases/Nicknames
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;
using Trades = std::vector<Trade>;


class OrderBook{
	private: 
        
		Trades trades_;

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

	       
		// Will this be inefficent since you're making a function call? It reduces repeating code but there's potential overhead 
		void fillOrders(auto& orderMap, const OrderPointer& incomingOrder){
		    
		    // Go through each order at each best price and fill each order and subtract their quantity from the market order
		    for (auto it = orderMap.begin(); it != orderMap.end() && incomingOrder->getRemainingQuantity() > 0;){
		
                Price currentPrice = it->first;

                // Limit order price constraint check
                // Gauranteed to match the best price but not the worst price so you need this check here 
                if (incomingOrder->getOrderType() == OrderType::Limit) {

                    if ((incomingOrder->getSide() == Side::Buy && currentPrice > incomingOrder->getPrice()) ||
                    (incomingOrder->getSide() == Side::Sell && currentPrice < incomingOrder->getPrice())) {
                        break; // You went in too deep 
                    }

                }


                OrderPointers& orderList = it->second;
                
                // Go through the map at each price with the value being all the orders FIFO at that price
                for (auto orderIt = orderList.begin(); orderIt != orderList.end() && incomingOrder->getRemainingQuantity() > 0; ){
                    OrderPointer& currentOrder = *orderIt;

                    Quantity quantityFilled = std::min(currentOrder->getRemainingQuantity(), incomingOrder->getRemainingQuantity());

                    incomingOrder->Fill(quantityFilled);
                    currentOrder->Fill(quantityFilled);
                    
                    // Record the trade in the order book 
                    TradeId tradeId{IdGenerator::generateTradeId()};                 
                    Trade trade
                    {
                        tradeId, 
                        incomingOrder->getOrderId(), 
                        currentOrder->getOrderId(), 
                        quantityFilled, 
                        currentOrder->getPrice()
                    };
                    trades_.push_back(trade);

                    // Trade(const TradeId& tradeId, const OrderId& buyOrderId, const OrderId& sellOrderId, const Quantity& quantity, const Price& price)
                    if(incomingOrder->getSide() == Side::Buy){
                        quantityOfAsks_ -= quantityFilled;
                    } else{
                        quantityOfBids_ -= quantityFilled;
                    }

                    if(currentOrder->isFilled()){
                        orderIt = orderList.erase(orderIt);
                    }
                
                }

                if(orderList.empty()){
                    it = orderMap.erase(it);
                }

		    }
		}

		void addOrderToOrderBook(auto& orderMap, const OrderPointer& incomingOrder){

		    // Gonna need to lock the map
		    OrderPointers& orderList = orderMap[incomingOrder->getPrice()];
		    orderList.push_back(incomingOrder);

		    const auto& it = std::prev(orderList.end());// Points to the actual last element and not the end cuz of prev
		    
		    // Maybe might also need to lock the unordered_map due to hash collisions? Not sure I think I don't 
		    OrderEntry orderEntry;
		    orderEntry.order_ = incomingOrder;
		    orderEntry.location_ = it;	
		    
		    orders_[incomingOrder->getOrderId()] = orderEntry;
		    // Only need to lock once i need it i believe

		    if(incomingOrder->getSide() == Side::Buy){
			    quantityOfBids_ += incomingOrder->getRemainingQuantity(); 
		    } else{
			    quantityOfAsks_ += incomingOrder->getRemainingQuantity();
		    }
		}

	public:

		[[nodiscard]] inline const Quantity& getQuantityOfAsks() const noexcept { return quantityOfAsks_; }
		[[nodiscard]] inline const Quantity& getQuantityOfBids() const noexcept { return quantityOfBids_; } 	
        
		void processOrder(const OrderPointer& incomingOrder){


			// TODO Find a way to better optimize the code (First see how the compiler compiles and if it doesn't optimize then you do it 
			// First determine the side of the order 
			Side incomingOrderSide = incomingOrder->getSide();

			if(incomingOrderSide == Side::Buy){
					
				Quantity quantityOfAsks = getQuantityOfAsks(); 
                
				// If market order check to see if the quantity can be filled or FOK 
				// Inherintly checks that the orderbook isn't empty  
                if(incomingOrder->getOrderType() == OrderType::Market){

                    if( incomingOrder->getRemainingQuantity() <= quantityOfAsks){
                        fillOrders(asks_, incomingOrder);
                        return;
                    }

                    return;
                }
				
				const Price* bestAsk = getBestAsk();

				// If the order book for asks is empty or the order is unable to match with best sell then add to orderbook	
				if( (quantityOfAsks == 0) || (*bestAsk > incomingOrder->getPrice()) ){
				    addOrderToOrderBook(bids_, incomingOrder);
				    return;
				}
				
				// Do matching logic here 
				fillOrders(asks_, incomingOrder);
				if(!incomingOrder->isFilled()){
				   addOrderToOrderBook(bids_, incomingOrder);
				}

			}

					
			if(incomingOrderSide == Side::Sell){
				
				Quantity quantityOfBids = getQuantityOfBids();

				// If the order type is market check to see if total quantity can be filled otherwise FOK
				// Inherintly checks that the order book isn't empty 
                if(incomingOrder->getOrderType() == OrderType::Market){

                    if( incomingOrder->getRemainingQuantity() <= quantityOfBids){
                        fillOrders(bids_, incomingOrder);
                        return;
                    }

                    return;

                }
				
				const Price* bestBid = getBestBid();

				// If the orderbook is empty or the order is unable to match with best sell then add to orderbook	
				if( (quantityOfBids == 0) || (*bestBid < incomingOrder->getPrice()) ){
				    addOrderToOrderBook(asks_, incomingOrder);
				    return;
				}

				// Do matching logic here 
				fillOrders(bids_, incomingOrder);
				if(!incomingOrder->isFilled()){
				   addOrderToOrderBook(asks_, incomingOrder);
				}
				

			}
	
		}
	

		// Cancel order and modify order see if you can change it to bool instead 	
		void modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price){}
		void cancelOrder(const OrderId& orderId){

			// Find the order if the order doesn't exist throw 
			const auto it = orders_.find(orderId);
			if(it == orders_.end()){
				throw std::runtime_error(std::format("Order ({}) doesn't exist", orderId));
			}

			
		}
		
		// Check to see if this can be inlined
		[[nodiscard]] const Price* getBestBid() const { 	
			if(!bids_.empty()){ 
				return &bids_.begin()->first;
			}		
			return nullptr;
		}


		//Check to see if this can be inlined 
		//Will return possible address of the pointer
		[[nodiscard]] const Price* getBestAsk() const { 
			if(!asks_.empty()){
				return &asks_.begin()->first;
			}	
		
			return nullptr;		
		}
};	


int main(){
    return 0;
}
