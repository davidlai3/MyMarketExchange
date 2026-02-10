CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2 -Iinclude
LDFLAGS  :=
LDLIBS   :=

BIN_DIR  := bin
OBJ_DIR  := build
SRC_DIR  := src

SERVER_EXE := $(BIN_DIR)/server
CLIENT_EXE := $(BIN_DIR)/client

# ---- pick which .cpp files belong to each executable ----
SERVER_SRCS := $(SRC_DIR)/main_server.cpp \
               $(SRC_DIR)/Server.cpp \
               $(SRC_DIR)/Exchange.cpp \
               $(SRC_DIR)/Orderbook.cpp \
               $(SRC_DIR)/OrderProtocol.cpp

CLIENT_SRCS := $(SRC_DIR)/main_client.cpp \
               $(SRC_DIR)/Client.cpp \
               $(SRC_DIR)/OrderProtocol.cpp

# Convert src/foo.cpp -> build/foo.o
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CLIENT_SRCS))

.PHONY: all server client clean
all: server client

server: $(SERVER_EXE)
client: $(CLIENT_EXE)

# Debug build: append flags, then rebuild
debug: CXXFLAGS += -g -O0 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer
debug: LDFLAGS  += -fsanitize=address,undefined
debug: clean all

# Link rules
$(SERVER_EXE): $(SERVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(CLIENT_EXE): $(CLIENT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compile rule (shared)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
