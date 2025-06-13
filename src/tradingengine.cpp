#include "../include/tradingengine.hpp"

#include <chrono>
#include <sstream>

TradingEngine::TradingEngine() : logger("trading_engine.log") {}

OrderID TradingEngine::submitOrder(const std::string& symbol, Side side, double price, int quantity) {
    OrderID id = nextOrderID++;
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    Order order(id, symbol, side, price, quantity, timestamp);
    book.add_order(order, logger);

    std::stringstream ss;
    ss << "Submitted order ID=" << order.id << " [" << symbol << " " 
       << ((side == Side::BUY) ? "BUY" : "SELL") << " "
       << quantity << " @ " << price << "]";
    logger.log(ss.str());

    return id;
}

bool TradingEngine::cancelOrder(OrderID id) {
    bool res = book.cancel_order(id);
    std::stringstream ss;
    if (res) ss << "Cancelled order ID=" << id;
    else ss << "Failed to cancel order ID=" << id;
    logger.log(ss.str());

    return res;
}

void TradingEngine::printOrderBook() {
    book.print_book();
}
