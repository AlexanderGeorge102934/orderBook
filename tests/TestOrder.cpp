#include <gtest/gtest.h>
#include "Order.h"
#include <stdexcept>


const Side SELL = Side::Sell;
const Side BUY = Side::Buy;
const Price TEST_PRICE = 100;
const OrderId TEST_ORDER_ID = 1;
const OrderType LIMIT = OrderType::Limit;

// Test for basic init
TEST(OrderTest, BasicInitialization) {
    Order order{BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, 10, 10};
    EXPECT_EQ(order.getPrice(), 100);
    EXPECT_FALSE(order.isFilled());
}

// Test for initial quantity less than 1
TEST(OrderTest, ThrowsWhenInitialQuantityIsLessThanOne) {
    Quantity invalidQuantity = 0;
    EXPECT_THROW({
        Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, invalidQuantity, invalidQuantity);
    }, std::invalid_argument);
}

// Test for remaining quantity less than 1
TEST(OrderTest, ThrowsWhenRemainingQuantityIsLessThanOne) {
    Quantity initialQuantity = 5;
    Quantity remainingQuantity = 0;
    EXPECT_THROW({
        Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, initialQuantity, remainingQuantity);
    }, std::invalid_argument);
}

// Can't have mismatch between initial and remaining quantity
TEST(OrderTest, ThrowsWhenQuantitiesDoNotMatchAtInitialization) {
    Quantity initialQuantity = 5;
    Quantity remainingQuantity = 4;
    EXPECT_THROW({
        Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, initialQuantity, remainingQuantity);
    }, std::invalid_argument);
}

// Make sure getter methods work 
TEST(OrderTest, GetterMethodsReturnCorrectValues) {
    Order order(BUY, 150, 42, LIMIT, 20, 20);
    EXPECT_EQ(order.getSide(), BUY);
    EXPECT_EQ(order.getPrice(), 150);
    EXPECT_EQ(order.getOrderId(), 42);
    EXPECT_EQ(order.getOrderType(), LIMIT);
    EXPECT_EQ(order.getInitialQuantity(), 20);
    EXPECT_EQ(order.getRemainingQuantity(), 20);
    EXPECT_EQ(order.getFilledQuantity(), 0);
}

// Test to see if filling properly reduces 
TEST(OrderTest, FillReducesRemainingQuantity) {
    Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, 10, 10);
    order.fill(3);
    EXPECT_EQ(order.getRemainingQuantity(), 7);
    EXPECT_EQ(order.getFilledQuantity(), 3);
}

// Check if fully filled works  
TEST(OrderTest, FullFillMarksOrderAsFilled) {
    Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, 5, 5);
    order.fill(5);
    EXPECT_TRUE(order.isFilled());
}

// Overfilling should throw a logic error 
TEST(OrderTest, FillThrowsIfQuantityExceedsRemaining) {
    Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, 5, 5);
    EXPECT_THROW(order.fill(6), std::logic_error);
}

// CHeck if multiple fills work  
TEST(OrderTest, MultipleFillsAccumulateCorrectly) {
    Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, 10, 10);
    order.fill(3);
    order.fill(2);
    EXPECT_EQ(order.getRemainingQuantity(), 5);
    EXPECT_EQ(order.getFilledQuantity(), 5);
    EXPECT_FALSE(order.isFilled());
}

// Edge case where filling zero results in throw 
TEST(OrderTest, FillZeroQuantityDoesNothing) {
    Quantity quantity = 10;
    Quantity zeroFill= 0;
    Order order(BUY, TEST_PRICE, TEST_ORDER_ID, LIMIT, quantity, quantity);
    EXPECT_THROW(order.fill(zeroFill), std::logic_error);
    EXPECT_EQ(order.getRemainingQuantity(), 10);
}
