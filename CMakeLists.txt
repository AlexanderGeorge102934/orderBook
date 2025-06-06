cmake_minimum_required(VERSION 3.14)

# My project name 
project(MyOrderBookProject)

# C++ Version 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header files 
include_directories(${CMAKE_SOURCE_DIR}/include)

# Subdirectories 
add_subdirectory(src) # Compile the cmake from the src dir as well 
add_subdirectory(external/googletest)
enable_testing()
add_subdirectory(tests)
