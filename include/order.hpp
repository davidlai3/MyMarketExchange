#pragma once
#include <cstdint>
#include <cstring>

enum class Side : uint8_t { BUY, SELL };
using OrderID = uint64_t;

struct Order {
    OrderID id;
    char symbol[16];
    Side side;
    double price;
    int quantity;
    uint64_t timestamp; // Unix timestamp in milliseconds

    Order(OrderID id_, const char* sym, Side s, double p, int q, uint64_t ts)
        : id(id_), side(s), price(p), quantity(q), timestamp(ts) {
            std::strncpy(symbol, sym, sizeof(symbol));
            symbol[sizeof(symbol) - 1] = '\0'; // Ensure null-termination
        }
    Order() = default;
};

struct OrderPacket {
    char symbol[16];
    Side side;
    double price;
    int quantity;

    void serialize(OrderPacket &packet, char* buffer) const;
    static OrderPacket deserialize(const char* buffer);
};
