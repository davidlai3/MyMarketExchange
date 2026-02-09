#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Types.h"

#include <map>
#include <deque>
#include <vector>
#include <unordered_map>

class Orderbook {
public:

    std::vector<Trade> add_order(const Order& order);
    bool cancel_order (uint32_t order_id);

private:

    // Currently we are using price-time priority as our matching algorithm.
    // We match orders at the passive price.

    std::map<uint32_t, std::deque<Order>, std::greater<uint32_t>> bids;
    std::map<uint32_t, std::deque<Order>> asks;

    std::vector<Trade> match_bid(Order aggressive);
    std::vector<Trade> match_ask(Order aggressive);

    struct OrderLocation {
        Side side;
        uint32_t price;
    };

    std::unordered_map<uint32_t, OrderLocation> order_locations;
};

#endif
