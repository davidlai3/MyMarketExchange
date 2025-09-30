#pragma once
#include "order.hpp"
#include "logger.hpp"

#include <map>
#include <deque>
#include <memory>
#include <unordered_map>
#include <optional>

class OrderBook {
    public:
        /* Adds an order to the order book. Orders are matched immediately via
         * price-time priority.
         * @param order The order to be added.
         * @param logger Logger instance for logging actions. */
        void add_order(std::shared_ptr<Order>&, Logger &logger);

        /* Cancels an order by its ID.
         * @param orderID The ID of the order to be canceled.
         * @return true if the order was successfully canceled, false if not found. */
        bool cancel_order(OrderID orderID);

        /* Prints the current state of the order book, showing bids and asks. */
        void print_book() const;

        /* Get total quantity at a specific price level for a given side
         * @param symbol The stock symbol.
         * @param side The side (BUY or SELL).
         * @param price The price level to check.
         * @return Total quantity at the specified price level.
         */
        int32_t get_total_quantity_at(const char* symbol, Side side, double price) const;

        /* Get the best bid and ask orders for a given symbol
         * @param symbol The stock symbol.
         * @return The best bid and ask orders.
         */
        std::optional<std::weak_ptr<const Order>> best_bid(const char* symbol) const;
        std::optional<std::weak_ptr<const Order>> best_ask(const char* symbol) const;

    private:
        // Data structures to hold bids and asks
        std::map<double, std::deque<std::shared_ptr<Order>>, std::greater<>> bids; // highest price first
        std::map<double, std::deque<std::shared_ptr<Order>>> asks;                 // lowest price first

        // Map to track orders by ID for quick access
        std::unordered_map<OrderID, std::weak_ptr<Order>> orderMap;

        /* Match incoming orders with existing ones
         * @param incoming shared_ptr to incoming order to match.
         * @param logger Logger instance for logging actions.
         * @param book book to search over (either bids or asks)
         */
        template<typename BookType> void match(std::shared_ptr<Order>& incoming, Logger &logger, BookType& book);
};
