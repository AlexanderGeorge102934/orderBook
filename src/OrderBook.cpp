#include "OrderBook.h"   
#include "Order.h"       
#include <map>          
#include <memory>        
#include <format>


template <typename OrderMap>
void OrderBook::fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder)
{ 
	// Go through each order at each best price and fill each order and subtract their quantity from the market order
	for (auto it {orderMap.begin()}; it != orderMap.end() && !incomingOrder->isFilled();){

		Price currentPrice {it->first};

		// Limit order price constraint check
		// Gauranteed to match the best price but not the worst price so you need this check here 
		if (incomingOrder->getOrderType() == OrderType::Limit &&
				((incomingOrder->getSide() == Side::Buy  && currentPrice > incomingOrder->getPrice()) ||
				 (incomingOrder->getSide() == Side::Sell && currentPrice < incomingOrder->getPrice())))
		{
			break; // You went in too deep
		}


		OrderPointers& orderList {it->second};

		// Go through the list of Order Ptrs at each price in the map with the value being all the orders FIFO at that price
		for (auto orderIt {orderList.begin()}; orderIt != orderList.end() && !incomingOrder->isFilled(); ){
			OrderPointer& currentOrder {*orderIt}; // Current order is an order that is already in the orderbook/Current order being pointed to

			Quantity quantityFilled {std::min(currentOrder->getRemainingQuantity(), incomingOrder->getRemainingQuantity())};

			incomingOrder->fill(quantityFilled);
			currentOrder->fill(quantityFilled);

			statusCache_[incomingOrder->getOrderId()].filledQuantity = incomingOrder->getFilledQuantity();
			statusCache_[currentOrder->getOrderId()].filledQuantity = currentOrder->getFilledQuantity();

			statusCache_[incomingOrder->getOrderId()].remainingQuantity = incomingOrder->getRemainingQuantity();
			statusCache_[currentOrder->getOrderId()].remainingQuantity = currentOrder->getRemainingQuantity();

			// Record the trade in the order book 
			TradeId tradeId{nextTradeId_++};                 
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
				statusCache_[currentOrder->getOrderId()].state = OrderState::Filled;
				statusCache_[currentOrder->getOrderId()].remainingQuantity = 0;
				statusCache_[currentOrder->getOrderId()].filledQuantity = currentOrder->getFilledQuantity();

				// Remove from the per-price list and also erase the order registry to avoid leaving a stale entry.
				OrderId filledOrderId = currentOrder->getOrderId();
				orderIt = orderList.erase(orderIt);        // erase returns iterator to next element
				orders_.erase(filledOrderId);              // remove entry from orders_
			}
			else{
				++orderIt;
			}
			// No need for else statement. If else then the for loop takes care of situation where incoming order got filled before current

		} // Inner for loop 

		if(orderList.empty()){
			it = orderMap.erase(it);
		}
		else{
			++it;
		}
		// No need for else statement. If else then the for loop takes care of situation where incoming order got filled before orderlist was empty 
	}

	if(incomingOrder->isFilled()){
		statusCache_[incomingOrder->getOrderId()].state = OrderState::Filled;
		statusCache_[incomingOrder->getOrderId()].remainingQuantity = 0;
		statusCache_[incomingOrder->getOrderId()].filledQuantity = incomingOrder->getFilledQuantity();
	}
}

template<typename OrderMap>
void OrderBook::addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder){
	const Price price {incomingOrder->getPrice()};

    auto [itMap, success] = orderMap.try_emplace(price);
    OrderPointers& orderList {itMap->second};

    orderList.push_back(incomingOrder);

    const auto itList = std::prev(orderList.end());
    orders_[incomingOrder->getOrderId()] = OrderEntry{incomingOrder, itList};

    if (incomingOrder->getSide() == Side::Buy) {
        quantityOfBids_ += incomingOrder->getRemainingQuantity(); 
    } else {
        quantityOfAsks_ += incomingOrder->getRemainingQuantity();
    }
}

bool OrderBook::processOrder(Order order)
{
	OrderId id = order.getOrderId();
	const auto it = statusCache_.find(id);
	if(it != statusCache_.end()){
		// Just return false why are you trying to redo an existing order?
		return false;
	}
	statusCache_.emplace(id, OrderStatus{order.getPrice(), order.getOrderType(), order.getSide(), OrderState::Processing, order.getInitialQuantity()});

	auto incomingOrder {std::allocate_shared<Order>(std::pmr::polymorphic_allocator<Order>(&pool_), order)};
	// First determine the side of the order 
	Side incomingOrderSide {incomingOrder->getSide()};

	if(incomingOrderSide == Side::Buy){

		Quantity quantityOfAsks {getQuantityOfAsks()};

		// If market order check to see if the quantity can be filled or FOK 
		// Inherintly checks that the orderbook isn't empty  
		if(incomingOrder->getOrderType() == OrderType::Market){

			if( incomingOrder->getRemainingQuantity() <= quantityOfAsks){
				fillOrders(asks_, incomingOrder);
				return true; // Market order filled
			}

			statusCache_[id].state = OrderState::Expired;
			return false; // Not enough orders to fill for market order 
		}

		const Price bestAsk {getBestAsk()};

		// If the order book for asks is empty or the order is unable to match with best sell then add to orderbook	
		if( (quantityOfAsks == 0) || (bestAsk > incomingOrder->getPrice()) ){
			addOrderToOrderBook(bids_, incomingOrder);
			//return true?
			return true; // Limit order posted to orderbook
		}

		// Do matching logic here 
		fillOrders(asks_, incomingOrder);
		if(!incomingOrder->isFilled()){// If not fully filled put in order book
			addOrderToOrderBook(bids_, incomingOrder);
		}

		return true; 

	}


	if(incomingOrderSide == Side::Sell){

		Quantity quantityOfBids {getQuantityOfBids()};

		// If the order type is market check to see if total quantity can be filled otherwise FOK
		// Inherintly checks that the order book isn't empty 
		if(incomingOrder->getOrderType() == OrderType::Market){

			if( incomingOrder->getRemainingQuantity() <= quantityOfBids){
				fillOrders(bids_, incomingOrder);
				return true; // Market order filled
			}

			statusCache_[id].state = OrderState::Expired;
			return false; // Not enough orders to fill for market order 

		}

		const Price bestBid {getBestBid()};

		// If the orderbook is empty or the order is unable to match with best sell then add to orderbook	
		if( (quantityOfBids == 0) || (bestBid < incomingOrder->getPrice()) ){
			addOrderToOrderBook(asks_, incomingOrder);
			return true; // limit order added to orderbook 
		}

		// Do matching logic here 
		fillOrders(bids_, incomingOrder);
		if(!incomingOrder->isFilled()){
			addOrderToOrderBook(asks_, incomingOrder);
		}   
		
		return true;

	}

	statusCache_[id].state = OrderState::Rejected;
	return false; 

}

bool OrderBook::cancelOrder(const OrderId& orderId) {
    const auto it = orders_.find(orderId);
    if (it == orders_.end()) {
        return false;
    }

    const auto& [id, entry] = *it; 
    const auto& orderPointer = entry.order_;
    const auto price = orderPointer->getPrice();
    const auto side = orderPointer->getSide();
    const auto quantity = orderPointer->getRemainingQuantity();

    if (side == Side::Buy) {
        auto mapIt = bids_.find(price);
        if (mapIt != bids_.end()) {
            mapIt->second.erase(entry.location_);
            quantityOfBids_ -= quantity;
            if (mapIt->second.empty()) {
                bids_.erase(mapIt);
            }
        }
    } else if(side == Side::Sell){
        auto mapIt = asks_.find(price);
        if (mapIt != asks_.end()) {
            mapIt->second.erase(entry.location_);
            quantityOfAsks_ -= quantity;
            if (mapIt->second.empty()) {
                asks_.erase(mapIt);
            }
        }
    }

    // Update status cache if necessary before deleting the order
    if (auto statusIt = statusCache_.find(orderId); statusIt != statusCache_.end()) {
        statusIt->second.state = OrderState::Cancelled;
    }

    orders_.erase(it);
    return true;
}

bool OrderBook::modifyOrder(const OrderId& orderId, const Quantity& quantity, const Price& price){
	const auto it = orders_.find(orderId);
	if(it == orders_.end()){
		return false;
	}

	OrderEntry orderEntry {it->second};

	OrderPointer& orderPointer {orderEntry.order_};

	Side side{orderPointer->getSide()};
	OrderType orderType {orderPointer->getOrderType()};

	//If false return false 
	if(!cancelOrder(orderId)){
		return false;
	}

	statusCache_.erase(orderId); // Tbh just for simplicity I'm keeping the same id 

	Order newOrder {Order(side, price, orderId, orderType, quantity, quantity)};

	if(!processOrder(newOrder)){
		return false;  
	}

	return true;
}

const OrderBook::OrderStatus OrderBook::reviewOrderStatus(const OrderId& orderId) const{
	auto it{statusCache_.find(orderId)};

	if(it != statusCache_.end()){
		return it->second;
	}

	return {0, OrderType::Unknown, Side::Unknown, OrderState::Rejected, 0, 0};
}

void OrderBook::display(size_t depth) const{ // Used gemini for this (too lazy to write my own display function)
	// 1. Safe Spread Calculation
	Price bestAsk = getBestAsk();
	Price bestBid = getBestBid();
	Price spread = (bestAsk == 0 || bestBid == 0) ? 0 : (bestAsk - bestBid);

	std::cout << "\n====================================\n";
	std::cout << "  ORDER BOOK (Top " << depth << " Levels)\n";
	std::cout << "====================================\n";
	std::cout << std::format("{:>10} | {:>10} | {:>10}\n", "Side", "Price", "Qty");
	std::cout << "------------------------------------\n";

	// 2. Display Asks (Top N cheapest sellers)
	// We want the 5 prices closest to the spread.
	// Since asks_ is sorted low-to-high, we take the first N elements and print in reverse
	size_t askCount = 0;
	std::vector<std::string> askLines;
	for (auto it = asks_.begin(); it != asks_.end() && askCount < depth; ++it, ++askCount)
	{
		Quantity totalQty = 0;
		for (const auto &order : it->second)
			totalQty += order->getRemainingQuantity();
		askLines.push_back(std::format("{:>10} | {:>10} | {:>10}", "ASK", it->first, totalQty));
	}
	// Print them high-to-low so the best ask is right above the spread
	for (auto it = askLines.rbegin(); it != askLines.rend(); ++it)
	{
		std::cout << *it << "\n";
	}

	std::cout << "---------- SPREAD: " << spread << " ----------\n";

	// 3. Display Bids (Top N highest buyers)
	size_t bidCount = 0;
	for (auto it = bids_.begin(); it != bids_.end() && bidCount < depth; ++it, ++bidCount)
	{
		Quantity totalQty = 0;
		for (const auto &order : it->second)
			totalQty += order->getRemainingQuantity();
		std::cout << std::format("{:>10} | {:>10} | {:>10}\n", "BID", it->first, totalQty);
	}
	std::cout << "====================================\n\n";
}