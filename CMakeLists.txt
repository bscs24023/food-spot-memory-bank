cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(food_spot_multiuser
    src/main_multiuser.cpp
    src/disk_database.cpp
    src/user_manager.cpp
    src/alert_system.cpp
    src/recommendation_system.cpp
)

add_executable(food_spot_disk
    src/main_disk.cpp
    src/disk_database.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")