#include <gtest/gtest.h>
#include "Trade.h"
#include <stdexcept>

const TradeId TEST_TRADE_ID = 1;
const OrderId TEST_ORDER_ID = 1;
const Price TEST_PRICE = 100;

TEST(TradeTest, BasicInitialization) {
    Trade trade{TEST_TRADE_ID, TEST_ORDER_ID, TEST_ORDER_ID, 100, TEST_PRICE};

    EXPECT_EQ(trade.getTradeId(), TEST_TRADE_ID);
    EXPECT_EQ(trade.getBuyOrderId(), TEST_ORDER_ID);
    EXPECT_EQ(trade.getSellOrderId(), TEST_ORDER_ID);
    EXPECT_EQ(trade.getQuantity(), 100);
    EXPECT_EQ(trade.getPrice(), TEST_PRICE);
}

TEST(TradeTest, ThrowsOnZeroQuantity) {
    Quantity invalidQuantity = 0;
    EXPECT_THROW({
        Trade trade(TEST_TRADE_ID, TEST_ORDER_ID, TEST_ORDER_ID, invalidQuantity, TEST_PRICE);
    }, std::invalid_argument);
}
