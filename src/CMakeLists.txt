add_library(orderbook
    OrderBook.cpp
)

add_executable(main
    main.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data.txt ${CMAKE_CURRENT_BINARY_DIR}/data.txt COPYONLY)