#pragma once
#include <string>

enum class Side { BUY, SELL }; 
using OrderID = uint64_t;

struct Order {
    OrderID id;
    std::string symbol;
    Side side;
    double price;
    int quantity;
    uint64_t timestamp; // Unix timestamp in milliseconds

    Order(OrderID id_, const std::string& sym, Side s, double p, int q, uint64_t ts)
        : id(id_), symbol(sym), side(s), price(p), quantity(q), timestamp(ts) {}
};
