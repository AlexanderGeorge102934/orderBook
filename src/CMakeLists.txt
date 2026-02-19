add_library(orderbook
    OrderBook.cpp
)

add_library(tradingsystem 
    TradingSystem.cpp
)

add_executable(main
    main.cpp
)

# This tells the Linker to pull in the code from libraries
target_link_libraries(main PRIVATE tradingsystem orderbook)

# boost asio can't use windows sockets off rip 
if(WIN32)
    target_link_libraries(main PRIVATE ws2_32 mswsock)
endif()
