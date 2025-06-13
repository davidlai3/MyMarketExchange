#include "../include/tradingengine.hpp"

int main() {
    TradingEngine engine;
    engine.submitOrder("AAPL", Side::BUY, 150.0, 10);
    engine.submitOrder("AAPL", Side::SELL, 155.0, 5);
    engine.submitOrder("AAPL", Side::BUY, 160.0, 2);
    engine.printOrderBook();
}
