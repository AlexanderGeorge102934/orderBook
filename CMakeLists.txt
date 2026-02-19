cmake_minimum_required(VERSION 3.14)

# My project name 
project(MyOrderBookProject)

# C++ Version 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_TESTING_SUITE "Build the testing suite" OFF) # Run with -DBUILD_TESTING_SUITE=ON to enable tests 

# Directories 
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Subdirectories 
add_subdirectory(src) 

# Only include tests and gtest if the option is enabled
if(BUILD_TESTING_SUITE)
    message(STATUS "Building with Testing Enabled")
    add_subdirectory(external/googletest)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Production Build: Testing disabled")
endif()