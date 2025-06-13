#include "../include/orderbook.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

void OrderBook::add_order(const Order &order, Logger &logger) {
    orderMap.emplace(order.id, order);
    match(order, logger);
}

bool OrderBook::cancel_order(OrderID orderID) {
    auto it = orderMap.find(orderID);
    if (it == orderMap.end()) {
        return false; // Order not found
    }
    double price = it->second.price;
    Side side = it->second.side;
    if (side == Side::BUY) {
        auto it2 = find(bids[price].begin(), bids[price].end(), orderID);
        if (it2 != bids[price].end()) {
            bids[price].erase(it2);
        } else {
            return false;
        }
    }
    else {
        auto it2 = find(asks[price].begin(), asks[price].end(), orderID);
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

/* TODO: Possibly change the order book to have deques store actual order objects instead
 * of just OrderIDs. This should speed the matching algorithm up, but may make order cancellation
 * more complicated. Also, find a way to avoid the code repetition in this algorithm. */
void OrderBook::match(const Order& incoming, Logger &logger) {
    double inc_price = incoming.price;
    int inc_quantity = incoming.quantity;
    Side side = incoming.side;
    if (side == Side::BUY) {
        for (auto &[price, queue] : asks) {
            if (price > inc_price) break;
            while (!queue.empty() && inc_quantity) {
                Order& matched_order = orderMap.at(queue.front());
                int min_quantity = std::min(inc_quantity, matched_order.quantity);
                matched_order.quantity -= min_quantity;
                inc_quantity -= min_quantity;

                std::stringstream ss;
                ss << "Trade executed: BUY " << min_quantity << " @ " << price << " between Order " 
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
            bids[inc_price].push_back(incoming.id);
        }
    } else {
        for (auto &[price, queue] : bids) {
            if (price < inc_price) break;
            while (!queue.empty() && inc_quantity) {
                Order& matched_order = orderMap.at(queue.front());
                int min_quantity = std::min(inc_quantity, matched_order.quantity);
                matched_order.quantity -= min_quantity;
                inc_quantity -= min_quantity;

                std::stringstream ss;
                ss << "Trade executed: SELL" << min_quantity << " @ " << price << " between Order " 
                   << incoming.id << " and Order " << matched_order.id << ".";
                logger.log(ss.str());

                // Order is fully matched
                if (matched_order.quantity <= 0) {
                    queue.pop_front();
                    orderMap.erase(matched_order.id);
                }
            }
            if (inc_quantity <= 0) break;
        }
        if (inc_quantity > 0) {
            asks[inc_price].push_back(incoming.id);
        }
    }
}
