#include <gtest/gtest.h>
#include <stdexcept>

#include <gtest/gtest.h>
#include "OrderBook.h"


namespace tradingValueConstants{

	constexpr Price INVALID_PRICE = std::numeric_limits<Price>::max();

}

// Ngl I'm tired so I just got this class from chat 
// I understand on a surface level but if you asked what was the motivation behind each part of the implementation
// I'm cooked  
class OrderBookTest : public ::testing::Test {
protected:
    OrderBook orderBook;
    OrderId nextOrderId = 1;

    // Used to create test orders
    OrderPointer makeOrder(Side side, Price price, Quantity quantity, OrderType type = OrderType::Limit) {
        return std::make_shared<Order>(side, price, nextOrderId++, type, quantity, quantity);
    }
};

TEST_F(OrderBookTest, AddBuyOrderUpdatesBestBid) {
    auto order = makeOrder(Side::Buy, 105, 10);  
    orderBook.processOrder(order);

    const Price* bestBid = orderBook.getBestBid();
    ASSERT_NE(bestBid, nullptr);
    EXPECT_EQ(*bestBid, 105);
    EXPECT_EQ(orderBook.getQuantityOfBids(), 10);
    EXPECT_EQ(order->getOrderId(), 1);
}


TEST_F(OrderBookTest, AddSellOrderUpdatesBestAsk) {
    auto order = makeOrder(Side::Sell, 110, 5);
    orderBook.processOrder(order);

    const Price* bestAsk = orderBook.getBestAsk();
    ASSERT_NE(bestAsk, nullptr);
    EXPECT_EQ(*bestAsk, 110);
    EXPECT_EQ(orderBook.getQuantityOfAsks(), 5);
}

TEST_F(OrderBookTest, CancelOrderRemovesOrder) {
    auto order = makeOrder(Side::Buy, 120, 20);
    orderBook.processOrder(order);

    EXPECT_EQ(*orderBook.getBestBid(), 120);
    EXPECT_EQ(orderBook.getQuantityOfBids(), 20);

    orderBook.cancelOrder(order->getOrderId());

    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getQuantityOfBids(), 0);
}

TEST_F(OrderBookTest, DoubleCancelOrderErrorHandle) {
    auto order = makeOrder(Side::Buy, 120, 20);
    orderBook.processOrder(order);

    EXPECT_NE(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getQuantityOfBids(), 20);

    orderBook.cancelOrder(order->getOrderId());
    EXPECT_THROW(orderBook.cancelOrder(order->getOrderId()), std::runtime_error);

}

TEST_F(OrderBookTest, ModifyOrderChangesPriceAndQuantity) {
    auto order = makeOrder(Side::Sell, 130, 15);
    orderBook.processOrder(order);

    EXPECT_EQ(*orderBook.getBestAsk(), 130);
    EXPECT_EQ(orderBook.getQuantityOfAsks(), 15);

    orderBook.modifyOrder(order->getOrderId(), 10, 125);

    EXPECT_EQ(*orderBook.getBestAsk(), 125);
    EXPECT_EQ(orderBook.getQuantityOfAsks(), 10);
}

TEST_F(OrderBookTest, MultipleOrdersUpdateBestBidAsk) {
    orderBook.processOrder(makeOrder(Side::Buy, 100, 10));
    orderBook.processOrder(makeOrder(Side::Buy, 110, 15));
    orderBook.processOrder(makeOrder(Side::Sell, 200, 5));
    orderBook.processOrder(makeOrder(Side::Sell, 190, 8));

    EXPECT_EQ(*orderBook.getBestBid(), 110);
    EXPECT_EQ(orderBook.getQuantityOfBids(), 25);

    EXPECT_EQ(*orderBook.getBestAsk(), 190);
    EXPECT_EQ(orderBook.getQuantityOfAsks(), 13);

    
}

TEST_F(OrderBookTest, CancelNonexistentOrderThrows) {
    EXPECT_THROW(orderBook.cancelOrder(9999), std::runtime_error);
}

TEST_F(OrderBookTest, ModifyNonexistentOrderThrows) {
    EXPECT_THROW(orderBook.modifyOrder(9999, 10, 100), std::runtime_error);
} 

TEST_F(OrderBookTest, OrderFillReducesQuantity) {
    auto sellOrder = makeOrder(Side::Sell, 100, 10);
    orderBook.processOrder(sellOrder);

    auto buyOrder = makeOrder(Side::Buy, 100, 6);
    orderBook.processOrder(buyOrder);

    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 100);


    EXPECT_EQ(orderBook.getQuantityOfBids(), 0);
    EXPECT_EQ(orderBook.getQuantityOfAsks(), 4);
}

TEST_F(OrderBookTest, TradesAreRecordedOnFill) {
    auto sellOrder = makeOrder(Side::Sell, 105, 10);
    orderBook.processOrder(sellOrder);

    auto buyOrder = makeOrder(Side::Buy, 105, 10);
    orderBook.processOrder(buyOrder);

    const auto& trades = orderBook.getTrades();
    EXPECT_EQ(trades.size(), 1);

    EXPECT_EQ(trades[0].getPrice(), 105);
    EXPECT_EQ(trades[0].getQuantity(), 10);

}

TEST_F(OrderBookTest, MultipleTradesAreRecordedOnFill) {
    
    // Trade happens at 105 for 5 
    // Asks: empty 
    // Bids: 110 for 5
    auto sellOrder = makeOrder(Side::Sell, 105, 5);
    orderBook.processOrder(sellOrder);
    auto buyOrder = makeOrder(Side::Buy, 110, 10);
    orderBook.processOrder(buyOrder);
    // -------------------------------------------- //

    // Trade happens for 110 at 5
    // Asks: empty 
    // Bids: empty 
    auto sellOrder2 = makeOrder(Side::Sell, 90, 5);
    orderBook.processOrder(sellOrder2);
    // -------------------------------------------- //

    // No trade happens 
    // Asks: empty 
    // Bids: 200 for 10 
    auto buyOrder2 = makeOrder(Side::Buy, 200, 10);
    orderBook.processOrder(buyOrder2);
    
    // Trade happens for 200 at 10
    // Asks: 180 at 1 
    // Bids: empty  
    auto sellOrder3 = makeOrder(Side::Sell, 180, 11);
    orderBook.processOrder(sellOrder3);
    // -------------------------------------------- //

    // Trade happens for 180 at 1
    // Asks: empty  
    // Bids: empty  
    auto buyOrder3 = makeOrder(Side::Buy, 200, 1);
    orderBook.processOrder(buyOrder3);
    // -------------------------------------------- //

    const auto& trades = orderBook.getTrades();
    EXPECT_EQ(trades.size(), 4);

    EXPECT_EQ(trades[0].getPrice(), 105);
    EXPECT_EQ(trades[0].getQuantity(), 5);

    EXPECT_EQ(trades[1].getPrice(), 110);
    EXPECT_EQ(trades[1].getQuantity(), 5);

    EXPECT_EQ(trades[2].getPrice(), 200);
    EXPECT_EQ(trades[2].getQuantity(), 10);

    EXPECT_EQ(trades[3].getPrice(), 180);
    EXPECT_EQ(trades[3].getQuantity(), 1);
}

TEST_F(OrderBookTest, MarketOrderOnEmptyOrderBook) {
    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 10, OrderType::Market);  
    orderBook.processOrder(order);

    EXPECT_EQ(order->getFilledQuantity(), 0);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}


TEST_F(OrderBookTest, MarketOrderOnNotEnoughQuantity) {
    auto askOrder = makeOrder(Side::Sell, 105, 5);  
    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 10, OrderType::Market);  
    orderBook.processOrder(askOrder);
    orderBook.processOrder(order);

    EXPECT_EQ(order->getFilledQuantity(), 0);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 105);
}

TEST_F(OrderBookTest, MarketOrderOnEnoughQuantity) {
    auto askOrder = makeOrder(Side::Sell, 105, 10);  
    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 10, OrderType::Market);  
    orderBook.processOrder(askOrder);
    orderBook.processOrder(order);

    EXPECT_EQ(order->isFilled(), true);
    EXPECT_EQ(order->getFilledQuantity(), 10);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}

TEST_F(OrderBookTest, MarketOrderOnMoreThanEnoughQuantity) {
    auto askOrder = makeOrder(Side::Sell, 105, 20);  
    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 10, OrderType::Market);  
    orderBook.processOrder(askOrder);
    orderBook.processOrder(order);

    EXPECT_EQ(order->isFilled(), true);
    EXPECT_EQ(order->getFilledQuantity(), 10);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 105);
}

TEST_F(OrderBookTest, MarketOrderMultipleOrdersFillEveryItem) {
    auto askOrder1 = makeOrder(Side::Sell, 105, 5);  
    auto askOrder2 = makeOrder(Side::Sell, 105, 10);  
    auto askOrder3 = makeOrder(Side::Sell, 105, 3);  

    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 18, OrderType::Market);  
    orderBook.processOrder(askOrder1);
    orderBook.processOrder(askOrder2);
    orderBook.processOrder(askOrder3);
    orderBook.processOrder(order);

    EXPECT_EQ(order->isFilled(), true);
    EXPECT_EQ(order->getFilledQuantity(), 18);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(orderBook.getBestAsk(), nullptr);
}

TEST_F(OrderBookTest, MarketOrderMultipleOrdersCannotFillEntireBook) {
    auto askOrder1 = makeOrder(Side::Sell, 105, 5);  
    auto askOrder2 = makeOrder(Side::Sell, 105, 10);  
    auto askOrder3 = makeOrder(Side::Sell, 105, 3);  

    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 15, OrderType::Market);  
    orderBook.processOrder(askOrder1);
    orderBook.processOrder(askOrder2);
    orderBook.processOrder(askOrder3);
    orderBook.processOrder(order);

    EXPECT_EQ(order->isFilled(), true);
    EXPECT_EQ(order->getFilledQuantity(), 15);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 105);
}

TEST_F(OrderBookTest, MarketOrderMultipleOrdersCannotFillEntireBookDifferentPrices) {
    auto askOrder1 = makeOrder(Side::Sell, 110, 5);  
    auto askOrder2 = makeOrder(Side::Sell, 90, 10);  
    auto askOrder3 = makeOrder(Side::Sell, 105, 3);  

    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 13, OrderType::Market);  
    orderBook.processOrder(askOrder1);
    orderBook.processOrder(askOrder2);
    orderBook.processOrder(askOrder3);
    orderBook.processOrder(order);

    EXPECT_EQ(order->isFilled(), true);
    EXPECT_EQ(order->getFilledQuantity(), 13);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 110);
}

TEST_F(OrderBookTest, MarketOrderMultipleOrdersAtDifferentTimes) {
    auto askOrder1 = makeOrder(Side::Sell, 110, 5);  
    auto askOrder2 = makeOrder(Side::Sell, 90, 10);  
    auto askOrder3 = makeOrder(Side::Sell, 105, 3);  

    auto order = makeOrder(Side::Buy, tradingValueConstants::INVALID_PRICE, 13, OrderType::Market);  
    orderBook.processOrder(askOrder1);
    orderBook.processOrder(order);
    EXPECT_EQ(order->isFilled(), false);

    orderBook.processOrder(askOrder2);
    orderBook.processOrder(order);
    EXPECT_EQ(order->isFilled(), true);

    orderBook.processOrder(askOrder3);

    EXPECT_EQ(order->getFilledQuantity(), 13);
    EXPECT_EQ(orderBook.getBestBid(), nullptr);
    EXPECT_EQ(*orderBook.getBestAsk(), 105);
}