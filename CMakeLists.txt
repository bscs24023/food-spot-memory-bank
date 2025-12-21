cmake_minimum_required(VERSION 3.10)
project(FoodSpotMemoryBank)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Multi-user application (NEW)
add_executable(food_spot_multiuser
    src/main_multiuser.cpp
    src/disk_database.cpp
    src/user_manager.cpp
    src/alert_system.cpp
)

# Single-user application (ORIGINAL - keep for backwards compatibility)
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
```

---

## ✅ FILE 8: .gitignore (Updated)
```
# Build files
build/
*.o
*.out
*.exe
*.a

# Data files
*.dat
data/users/*.dat
data/system/*.dat

# VS Code
.vscode/

# System files
*.swp
*~
.DS_Store
Thumbs.db

# Windows
Thumbs.db
desktop.ini

# Mac
.DS_Store