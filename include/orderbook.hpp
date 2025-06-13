#pragma once
#include "order.hpp"
#include "logger.hpp"

#include <map>
#include <deque>
#include <unordered_map>

class OrderBook {
    public:
        /* Adds an order to the order book.
         * @param order The order to be added.
         * @param logger Logger instance for logging actions. */
        void add_order(const Order &order, Logger &logger);

        /* Cancels an order by its ID.
         * @param orderID The ID of the order to be canceled.
         * @return true if the order was successfully canceled, false if not found. */
        bool cancel_order(OrderID orderID);

        /* Prints the current state of the order book, showing bids and asks. */
        void print_book() const;
    private:
        // Data structures to hold bids and asks
        std::map<double, std::deque<OrderID>, std::greater<>> bids; // highest price first
        std::map<double, std::deque<OrderID>> asks;                 // lowest price first

        // Map to track orders by ID for quick access
        std::unordered_map<OrderID, Order> orderMap;

        /* Match incoming orders with existing ones
         * @param incoming The incoming order to match.
         * @param logger Logger instance for logging actions. */
        void match(const Order &incoming, Logger &logger);
};
