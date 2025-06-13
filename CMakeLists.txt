cmake_minimum_required(VERSION 3.10)
project(TradingEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/orderbook.cpp
    src/tradingengine.cpp
    src/logger.cpp
)

# Add the executable
add_executable(trading_engine ${SOURCES})

# Optional: enable warnings
target_compile_options(trading_engine PRIVATE -Wall -Wextra -Wpedantic)
