cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Test executable
add_executable(test_data_structures
    tests/test_data_structures.cpp
)

# Enable warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")