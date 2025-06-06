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

# Build for testing Order Book 

add_executable(TestOrderBook 
    TestOrderBook.cpp
    ${PROJECT_SOURCE_DIR}/src/OrderBook.cpp
)

target_link_libraries(TestOrderBook
    gtest
    gtest_main
)



gtest_discover_tests(TestOrderBook)
