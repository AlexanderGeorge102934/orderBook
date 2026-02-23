include(GoogleTest)

# Build for testing Order 
add_executable(TestOrder
    TestOrder.cpp
)


target_link_libraries(TestOrder
    gtest
    gtest_main
)

gtest_discover_tests(TestOrder)

# Build for testing Trade

add_executable(TestTrade 
    TestTrade.cpp
)

target_link_libraries(TestTrade
    gtest
    gtest_main
)

gtest_discover_tests(TestTrade)

#Build for testing Order Book 

  add_executable(TestOrderBook 
    TestOrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/OrderBook.cpp
)

target_link_libraries(TestOrderBook
    gtest
    gtest_main
)

gtest_discover_tests(TestOrderBook)

# Build for testing Pipeline

add_executable(TestPipeline
    TestPipeline.cpp
)

target_link_libraries(TestPipeline
    gtest
    gtest_main
)

gtest_discover_tests(TestPipeline)

# Build for testing Trading System
add_executable(TestTradingSystem
    TestTradingSystem.cpp
)

# Link the standard Google Test libraries first
target_link_libraries(TestTradingSystem PRIVATE
    gtest
    gtest_main
)

# Now check for Windows and link the socket libraries separately
if(WIN32)
    target_link_libraries(TestTradingSystem PRIVATE ws2_32 mswsock)
endif()

gtest_discover_tests(TestTradingSystem)

