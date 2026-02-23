#include <gtest/gtest.h>
#include "TradingSystem.h"
#include <thread>
#include <chrono>

class TradingSystemTest : public ::testing::Test {

};

// Test that TradingSystem initializes successfully
TEST_F(TradingSystemTest, TradingSystemInitializesSuccessfully) {
    EXPECT_NO_THROW({
        TradingSystem tradingSystem;
    });
}

// Test that TradingSystem destructor completes successfully
TEST_F(TradingSystemTest, TradingSystemDestructorCompletesSuccessfully) {
    EXPECT_NO_THROW({
        {
            TradingSystem tradingSystem;
        } // Destructor called here
    });
}

// Test that multiple TradingSystem instances can be created
TEST_F(TradingSystemTest, MultipleTradingSystemInstancesCanBeCreated) {
    EXPECT_NO_THROW({
        TradingSystem ts1;
        TradingSystem ts2;
    });
}

// Test that TradingSystem is non-copyable
TEST_F(TradingSystemTest, TradingSystemIsNonCopyable) {
    static_assert(!std::is_copy_constructible_v<TradingSystem>, "TradingSystem should not be copy constructible");
    static_assert(!std::is_copy_assignable_v<TradingSystem>, "TradingSystem should not be copy assignable");
}

// Test that TradingSystem is non-movable
TEST_F(TradingSystemTest, TradingSystemIsNonMovable) {
    static_assert(!std::is_move_constructible_v<TradingSystem>, "TradingSystem should not be move constructible");
    static_assert(!std::is_move_assignable_v<TradingSystem>, "TradingSystem should not be move assignable");
}

// Test that TradingSystem rapid construction/destruction works
TEST_F(TradingSystemTest, RapidConstructionDestructionCycles) {
    EXPECT_NO_THROW({
        for (int i = 0; i < 5; ++i) {
            TradingSystem tradingSystem;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
}