#include "../include/orderbook.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>

void OrderBook::add_order(std::shared_ptr<Order>& order, Logger &logger) {
    std::weak_ptr<Order> wptr = order;
    orderMap.emplace(order->id, wptr);
    if (order->side == Side::BUY) {
        match(order, logger, asks);
    }
    else if (order->side == Side::SELL) {
        match(order, logger, bids);
    }
}

bool OrderBook::cancel_order(OrderID orderID) {
    auto it = orderMap.find(orderID);
    if (it == orderMap.end()) {
        return false; // Order not found
    }

    // Remove order from the appropriate book
    auto sp = it->second.lock();
    if (!sp) {
        return false; // Order already expired
    }

    double price = sp->price;
    Side side = sp->side;
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
    sp.reset(); // Free order from memory

    return true;
}

void OrderBook::print_book() const {

    auto get_total_quantity = [](int32_t sum, const Order& o) {
        return sum + o.quantity;
    };

    std::cout << "ASKS:\n";
    for (const auto& [price, queue] : asks) {
        int32_t total_quantity = std::accumulate(queue.begin(), queue.end(), 0, get_total_quantity);
        std::cout << "Price: " << price << " (" << total_quantity << " total quantity, "
              << queue.size() << " orders)\n";
    }

    std::cout << "BIDS:\n";
    for (const auto& [price, queue] : bids) {
        int32_t total_quantity = std::accumulate(queue.begin(), queue.end(), 0, get_total_quantity);
        std::cout << "Price: " << price << " (" << total_quantity << " total quantity, "
              << queue.size() << " orders)\n";
    }
}

int32_t OrderBook::get_total_quantity_at(const char* symbol, Side side, double price) const {

    auto get_total_quantity = [](int32_t sum, const Order& o) {
        return sum + o.quantity;
    };

    if (side == Side::BUY) {
        auto it = bids.find(price);
        if (it == bids.end()) {
            return 0;
        }
        return std::accumulate(it->second.begin(), it->second.end(), 0, get_total_quantity);
    } else {
        auto it = asks.find(price);
        if (it == asks.end()) {
            return 0;
        }
        return std::accumulate(it->second.begin(), it->second.end(), 0, get_total_quantity);
    }
}

std::optional<std::weak_ptr<const Order>> OrderBook::best_bid(const char* symbol) const {
    for (auto it = bids.begin(); it != bids.end(); ++it) {
        if (!it->second.empty()) {
            return it->second.front();
        }
    }
    return std::nullopt;
}

std::optional<std::weak_ptr<const Order>> OrderBook::best_ask(const char* symbol) const {
    for (auto it = asks.begin(); it != asks.end(); ++it) {
        if (!it->second.empty()) {
            return it->second.front();
        }
    }
    return std::nullopt;
}

/* We use a template here because the two books are not of the same type.
* One has a greater comparator, the other does not. */
template<typename BookType>
void OrderBook::match(std::shared_ptr<Order>& incoming, Logger& logger, BookType& book) {
    const double inc_price = incoming->price;
    int32_t inc_qty = incoming->quantity;

    auto crosses = [&](double level) {
        return (incoming->side == Side::BUY) ? (level <= inc_price)
                                             : (level >= inc_price);
    };
    auto should_break = [&](double level) {
        return (incoming->side == Side::BUY) ? (level > inc_price)
                                             : (level < inc_price);
    };

    for (auto it = book.begin(); it != book.end() && !should_break(it->first); ) {
        auto& queue = it->second;
        while (!queue.empty() && inc_qty > 0 && crosses(it->first)) {
            std::shared_ptr<Order> resting = queue.front();

            const int32_t fill = std::min(inc_qty, resting->quantity);
            resting->quantity -= fill;
            inc_qty -= fill;

            std::stringstream ss;
            ss << "Trade executed: " << (incoming->side == Side::BUY ? "BUY " : "SELL ")
               << fill << " @ " << it->first << " between Order "
               << incoming->id << " and Order " << resting->id << ".";
            logger.log(ss.str());

            if (resting->quantity == 0) {
                queue.pop_front();
                orderMap.erase(resting->id);
            }
        }

        // erase empty price levels as you go
        if (queue.empty()) it = book.erase(it);
        else ++it;

        if (inc_qty == 0) break;
    }

    if (inc_qty > 0) {
        incoming->quantity = inc_qty;
        book[inc_price].push_back(incoming);
    }
}

