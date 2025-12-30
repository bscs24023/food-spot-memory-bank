cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find Pybind11
find_package(pybind11 REQUIRED)

# Build the multiuser executable
add_executable(food_spot_multiuser
    src/main_multiuser.cpp
    src/disk_database.cpp
    src/user_manager.cpp
    src/alert_system.cpp
    src/recommendation_system.cpp
)

# Build the Pybind11 wrapper module
pybind11_add_module(py_foodspot
    src/pybind_wrapper.cpp
    src/disk_database.cpp
    src/user_manager.cpp
    src/alert_system.cpp
    src/recommendation_system.cpp
)

# Enable compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
