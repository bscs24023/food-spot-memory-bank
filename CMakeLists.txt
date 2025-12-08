cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(food_spot_disk
    src/main_disk.cpp
    src/disk_database.cpp
)

add_executable(test_data_structures
    tests/test_data_structures.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")