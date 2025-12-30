cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Multi-user application with recommendations
add_executable(food_spot_multiuser
    src/main_multiuser.cpp
    src/disk_database.cpp
    src/user_manager.cpp
    src/alert_system.cpp
    src/recommendation_system.cpp
)

# Original single-user application (keep for reference)
add_executable(food_spot_disk
    src/main_disk.cpp
    src/disk_database.cpp
)

# Test suite
add_executable(test_data_structures
    tests/test_data_structures.cpp
)

# Enable warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")