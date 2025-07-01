#pragma once

#include "orderbook.hpp"
#include "logger.hpp"

class TradingEngine {
    public:
        TradingEngine();
        OrderID submitOrder(const char*, Side side, double price, int quantity);
        bool cancelOrder(OrderID id);
        void printOrderBook();
    private:
        std::mutex mtx;
        OrderBook book;
        OrderID nextOrderID = 1;
        Logger logger;
};
