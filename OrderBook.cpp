#include "OrderBook.h"

template <typename OrderMap>
void OrderBook::fillOrders(OrderMap& orderMap, const OrderPointer& incomingOrder)
{ 
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

template<typename OrderMap>
void OrderBook::addOrderToOrderBook(OrderMap& orderMap, const OrderPointer& incomingOrder){

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



void OrderBook::processOrder(const OrderPointer& incomingOrder)
{
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
