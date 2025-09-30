# MyMarketExchange

A high-performance C++ trading engine and market exchange system that allows clients to connect and submit orders for real-time trading operations.

## Overview

MyMarketExchange is a complete trading system implementation featuring:
- **Order Book Management**: Efficient order matching with price-time priority
- **Trading Engine**: Core trading logic with order submission and cancellation
- **Network Server**: TCP-based client-server architecture for remote trading
- **Real-time Matching**: Immediate order execution with FIFO priority at price levels
- **Thread-safe Operations**: Concurrent client handling with proper synchronization

## Architecture

### Core Components

1. **TradingEngine** (`include/tradingengine.hpp`, `src/tradingengine.cpp`)
   - Central trading logic
   - Order submission and cancellation
   - Order book management
   - Thread-safe operations

2. **OrderBook** (`include/orderbook.hpp`, `src/orderbook.cpp`)
   - Order storage and matching engine
   - Price-time priority matching
   - Bid/ask management with efficient data structures

3. **TradingServer** (`include/server.hpp`, `src/server.cpp`)
   - TCP server for client connections
   - Multi-threaded client handling
   - Order packet serialization/deserialization

4. **Order Management** (`include/order.hpp`)
   - Order data structures
   - Network packet format
   - Serialization utilities

5. **Logging System** (`include/logger.hpp`, `src/logger.cpp`)
   - Comprehensive trading activity logging
   - Order lifecycle tracking

## Features

### Order Management
- **Order Types**: Market orders with immediate execution
- **Order Sides**: Buy and Sell orders
- **Order Matching**: Price-time priority with FIFO at same price level
- **Order Cancellation**: Cancel orders by ID
- **Symbol Support**: Multi-symbol trading (e.g., AAPL, MSFT, etc.)

### Trading Engine Capabilities
- **Real-time Matching**: Orders are matched immediately upon submission
- **Price Discovery**: Best bid/ask price tracking
- **Quantity Tracking**: Total quantity at specific price levels
- **Order Book Visualization**: Print current market state

### Network Features
- **TCP Server**: Handles multiple concurrent clients
- **Order Serialization**: Binary packet format for efficient transmission
- **Client Management**: Up to 10 concurrent client connections
- **Error Handling**: Robust connection and data handling

## Building the Project

### Prerequisites
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- CMake 3.10 or higher
- Linux/Unix environment (uses POSIX sockets)

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd MyMarketExchange

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make

# Build tests
make trading_tests
```

### Executables Generated
- `trading_engine`: Main server executable
- `trading_tests`: Test suite executable

## Usage

### Starting the Trading Server

```bash
# Run the server (default port 8080)
./trading_engine

# The server will start listening for client connections
# Output: "Server initialized on 8080"
# Output: "Listening for clients..."
```

### Client Connection

The system includes a sample client (`src/client.cpp`) that demonstrates how to connect and send orders:

```bash
# Compile and run the client
g++ -o client src/client.cpp
./client
```

### Order Submission

Orders are submitted through the network interface using the `OrderPacket` format:

```cpp
struct OrderPacket {
    char symbol[16];    // Stock symbol (e.g., "AAPL")
    Side side;          // BUY or SELL
    double price;       // Order price
    int quantity;       // Order quantity
};
```

### Example Trading Operations

```cpp
// Create trading engine instance
TradingEngine engine;

// Submit buy order for AAPL at $150.00 for 10 shares
engine.submitOrder("AAPL", Side::BUY, 150.0, 10);

// Submit sell order for AAPL at $155.00 for 5 shares
engine.submitOrder("AAPL", Side::SELL, 155.0, 5);

// Print current order book state
engine.printOrderBook();

// Get best bid and ask
auto bestBid = engine.bestBid("AAPL");
auto bestAsk = engine.bestAsk("AAPL");
```

## Testing

The project includes comprehensive tests for order book functionality:

```bash
# Run the test suite
./trading_tests
```

### Test Coverage
- **Non-crossing orders**: Orders that don't immediately match
- **Partial fills**: Orders that partially match existing orders
- **Order sweeping**: Orders that completely consume existing orders
- **FIFO priority**: First-in-first-out at same price level
- **Best bid/ask**: Price discovery functionality

## API Reference

### TradingEngine Class

```cpp
class TradingEngine {
public:
    // Submit a new order
    OrderID submitOrder(const char* symbol, Side side, double price, int quantity);
    
    // Cancel an existing order
    bool cancelOrder(OrderID id);
    
    // Get best bid for a symbol
    Order bestBid(const char* symbol);
    
    // Get best ask for a symbol
    Order bestAsk(const char* symbol);
    
    // Get total quantity at specific price level
    int32_t totalQtyAt(const char* symbol, Side side, double price);
    
    // Print current order book state
    void printOrderBook();
};
```

### OrderBook Class

```cpp
class OrderBook {
public:
    // Add order to the book
    void add_order(std::shared_ptr<Order>& order, Logger &logger);
    
    // Cancel order by ID
    bool cancel_order(OrderID orderID);
    
    // Print order book state
    void print_book() const;
    
    // Get best bid/ask
    std::optional<std::weak_ptr<const Order>> best_bid(const char* symbol) const;
    std::optional<std::weak_ptr<const Order>> best_ask(const char* symbol) const;
};
```

## Performance Characteristics

- **Order Matching**: O(log n) insertion with immediate matching
- **Memory Usage**: Efficient storage using STL containers
- **Concurrency**: Thread-safe operations with mutex protection
- **Network**: Low-latency TCP communication
- **Scalability**: Supports multiple concurrent clients

## Future Enhancements

The current implementation provides a solid foundation for a trading system. Potential enhancements include:

- **Order Types**: Limit orders, stop orders, market orders
- **Market Data**: Real-time price feeds and market data distribution
- **Persistence**: Order book persistence and recovery
- **Risk Management**: Position limits and risk controls
- **Advanced Matching**: Iceberg orders, hidden orders
- **Performance**: Lock-free data structures for higher throughput
- **Monitoring**: Web-based dashboard and monitoring tools
