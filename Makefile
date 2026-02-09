# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = bin

# Target executable
TARGET = bin/MyMarketExchange

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Convert src/foo.cpp -> build/foo.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
