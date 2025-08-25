#include "../include/orderbook.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

void OrderBook::add_order(Order &order, Logger &logger) {
    orderMap.emplace(order.id, order);
    if (order.side == Side::BUY) {
        match(order, logger, asks);
    }
    else if (order.side == Side::SELL) {
        match(order, logger, bids);
    }
}

bool OrderBook::cancel_order(OrderID orderID) {
    auto it = orderMap.find(orderID);
    if (it == orderMap.end()) {
        return false; // Order not found
    }

    // Remove order from the appropriate book
    double price = it->second.price;
    Side side = it->second.side;
    if (side == Side::BUY) {
        auto it2 = find_if(bids[price].begin(), bids[price].end(),
                [orderID](const Order& o) { return o.id == orderID; });
        if (it2 != bids[price].end()) {
            bids[price].erase(it2);
        } else {
            return false;
        }
    }
    else {
        auto it2 = find_if(asks[price].begin(), asks[price].end(),
                [orderID](const Order& o) { return o.id == orderID; });
        if (it2 != asks[price].end()) {
            asks[price].erase(it2);
        } else {
            return false;
        }
    }

    orderMap.erase(it);
    return true;
}

void OrderBook::print_book() const {
    std::cout << "ASKS:\n";
    for (const auto& [price, queue] : asks) {
        std::cout << "Price: " << price << " (" << queue.size() << " orders)\n";
    }

    std::cout << "BIDS:\n";
    for (const auto& [price, queue] : bids) {
        std::cout << "Price: " << price << " (" << queue.size() << " orders)\n";
    }
}

/* We must use a template here because the two books are not of the same type.
* One has a greater comparator, the other does not. */
template<typename BookType>
void OrderBook::match(Order& incoming, Logger& logger, BookType& book) {
    double inc_price = incoming.price;
    int inc_quantity = incoming.quantity;

    // Price-time priority matching
    for (auto& [price, queue] : book) {
        if (price > inc_price) break;
        while (!queue.empty() && inc_quantity) {
            Order& matched_order = queue.front();
            int min_quantity = std::min(inc_quantity, matched_order.quantity);
            matched_order.quantity -= min_quantity;
            inc_quantity -= min_quantity;

            std::stringstream ss;
            std::string action = incoming.side == Side::BUY ? "BUY " : "SELL ";
            ss << "Trade executed: " << action << min_quantity << " @ " << price << " between Order " 
               << incoming.id << " and Order " << matched_order.id << ".";
            logger.log(ss.str());

            // Order is fully matched
            if (matched_order.quantity <= 0) {
                queue.pop_front();
                orderMap.erase(matched_order.id);
            }
        }
        if (inc_quantity <= 0) break; // Incoming order is completely filled
    }
    if (inc_quantity > 0) {
        incoming.quantity = inc_quantity; // Update incoming order's quantity
        book[inc_price].push_back(incoming);
    }
}
