#include "OrderBook.h"   
#include "Order.h"       
#include <map>          
#include <memory>        
#include <format>


	template <typename OrderMap>
void OrderBook::fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder)
{ 
	// Go through each order at each best price and fill each order and subtract their quantity from the market order
	for (auto it = orderMap.begin(); it != orderMap.end() && incomingOrder->getRemainingQuantity() > 0;){

		Price currentPrice {it->first};

		// Limit order price constraint check
		// Gauranteed to match the best price but not the worst price so you need this check here 
		if (incomingOrder->getOrderType() == OrderType::Limit &&
				((incomingOrder->getSide() == Side::Buy  && currentPrice > incomingOrder->getPrice()) ||
				 (incomingOrder->getSide() == Side::Sell && currentPrice < incomingOrder->getPrice())))
		{
			break; // You went in too deep
		}


		OrderPointers& orderList = it->second;

		// Go through the list of Order Ptrs at each price in the map with the value being all the orders FIFO at that price
		for (auto orderIt {orderList.begin()}; orderIt != orderList.end() && incomingOrder->getRemainingQuantity() > 0; ){
			OrderPointer& currentOrder {*orderIt};

			Quantity quantityFilled {std::min(currentOrder->getRemainingQuantity(), incomingOrder->getRemainingQuantity())};

			incomingOrder->fill(quantityFilled);
			currentOrder->fill(quantityFilled);

			// Record the trade in the order book 
			TradeId tradeId{nextTradeId++};                 
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
				// Remove from the per-price list and also erase the order registry to avoid leaving a stale entry.
				OrderId filledOrderId = currentOrder->getOrderId();
				orderIt = orderList.erase(orderIt);        // erase returns iterator to next element
				orders_.erase(filledOrderId);              // remove entry from orders_
			}
			// No need for else statement. If else then the for loop takes care of situation where incoming order got filled before current

		} // Inner for loop 

		if(orderList.empty()){
			it = orderMap.erase(it);
		}
		// No need for else statement. If else then the for loop takes care of situation where incoming order got filled before orderlist was empty 
	}
}

template<typename OrderMap>
void OrderBook::addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder){

	OrderPointers& orderList = orderMap[incomingOrder->getPrice()];
	orderList.push_back(incomingOrder);

	const auto& it = std::prev(orderList.end());// Points to the actual last element and not the end cuz of prev

	OrderEntry orderEntry;
	orderEntry.order_ = incomingOrder;
	orderEntry.location_ = it;	

	orders_[incomingOrder->getOrderId()] = orderEntry;

	if(incomingOrder->getSide() == Side::Buy){
		quantityOfBids_ += incomingOrder->getRemainingQuantity(); 
	} else{
		quantityOfAsks_ += incomingOrder->getRemainingQuantity();
	}
}



void OrderBook::processOrder(const OrderPointer& incomingOrder)
{
	// TODO Find a way to better optimize the code (First see how the compiler compiles and if it doesn't optimize then you do it 
	// First determine the side of the order 
	Side incomingOrderSide {incomingOrder->getSide()};

	if(incomingOrderSide == Side::Buy){

		Quantity quantityOfAsks {getQuantityOfAsks()};

		// If market order check to see if the quantity can be filled or FOK 
		// Inherintly checks that the orderbook isn't empty  
		if(incomingOrder->getOrderType() == OrderType::Market){

			if( incomingOrder->getRemainingQuantity() <= quantityOfAsks){
				fillOrders(asks_, incomingOrder);
				return;
			}

			return;
		}

		const Price* bestAsk {getBestAsk()};

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

		Quantity quantityOfBids {getQuantityOfBids()};

		// If the order type is market check to see if total quantity can be filled otherwise FOK
		// Inherintly checks that the order book isn't empty 
		if(incomingOrder->getOrderType() == OrderType::Market){

			if( incomingOrder->getRemainingQuantity() <= quantityOfBids){
				fillOrders(bids_, incomingOrder);
				return;
			}

			return;

		}

		const Price* bestBid {getBestBid()};

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

void OrderBook::cancelOrder(const OrderId& orderId) {
	const auto it = orders_.find(orderId);
	if (it == orders_.end()) {
		throw std::runtime_error(std::format("Order ({}) doesn't exist", orderId));
	}

	const OrderEntry& orderEntry {it->second};
	const OrderPointer& orderPointer {orderEntry.order_};
	const auto& location = orderEntry.location_;

	const Side side {orderPointer->getSide()};
	const Price price {orderPointer->getPrice()};
	const Quantity quantity {orderPointer->getRemainingQuantity()};

	if (side == Side::Buy) {
		bids_[price].erase(location);
		quantityOfBids_ -= quantity;
		if (bids_[price].empty()) {
			bids_.erase(price);
		}
	} else {
		asks_[price].erase(location);
		quantityOfAsks_ -= quantity;
		if (asks_[price].empty()) {
			asks_.erase(price);
		}
	}

	orders_.erase(it);
}

void OrderBook::modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price){
	const auto it = orders_.find(orderId);
	if(it == orders_.end()){
		throw std::runtime_error(std::format("Order ({}) doesn't exist", orderId));
	}

	OrderEntry orderEntry {it->second};

	OrderPointer& orderPointer {orderEntry.order_};

	Side side{orderPointer->getSide()};
	OrderType orderType {orderPointer->getOrderType()};

	cancelOrder(orderId);

	OrderPointer newOrderPointer {std::make_shared<Order>(side, price, orderId, orderType, quantity, quantity)};


	processOrder(newOrderPointer);


}


