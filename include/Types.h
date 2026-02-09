#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <ctime>
#include <chrono>
#include <atomic>

enum class Side {BUY, SELL};
using timestamp_t = uint64_t;

// Order id generator
static inline uint32_t next_order_id() {
    static std::atomic<uint32_t> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

// Trade id generator
static inline uint32_t next_trade_id() {
    static std::atomic<uint32_t> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

// Timestamp generator
static inline timestamp_t current_timestamp() {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

struct Order {
    uint32_t id;
    Side side;
    uint32_t price;
    uint32_t volume;
    timestamp_t timestamp;

    Order() = default;

    Order(Side side_, uint32_t price_, uint32_t volume_)
        : id(next_order_id()), 
          side(side_), 
          price(price_), 
          volume(volume_), 
          timestamp(current_timestamp()) {}

    Order(uint32_t id_, Side side_, uint32_t price_, uint32_t volume_, timestamp_t ts_)
        : id(id_),
          side(side_),
          price(price_),
          volume(volume_),
          timestamp(ts_) {}
};

struct Trade {
    uint32_t id;
    uint32_t buy_id;
    uint32_t sell_id;
    uint32_t price;
    uint32_t quantity;
    timestamp_t timestamp;
    
    Trade(uint32_t buy_id_, uint32_t sell_id_, uint32_t price_, uint32_t quantity_)
        : id(next_trade_id()),
          buy_id(buy_id_), 
          sell_id(sell_id_), 
          price(price_), 
          quantity(quantity_),
          timestamp(current_timestamp()) {}
};

#endif
