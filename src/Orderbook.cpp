#include "../include/Orderbook.h"

std::vector<Trade> Orderbook::add_order(const Order& order) {
    if (order.side == Side::BUY) {
        return match_bid(order);
    }
    else {
        return match_ask(order);
    }
}

bool Orderbook::cancel_order(uint32_t order_id) {

    std::deque<Order>* orders;

    Side side = order_locations[order_id].side;
    uint32_t price = order_locations[order_id].price;

    if (side == Side::BUY) {
        orders = &bids[price];
    }
    else {
        orders = &asks[price];
    }

    bool erased = false;
    for (auto it = (*orders).begin(); it != (*orders).end(); it++) {
        if (it->id == order_id) {
            erased = true;
            orders->erase(it);
            break;
        }
    }

    order_locations.erase(order_id);

    return erased;
}

std::vector<Trade> Orderbook::match_bid(Order aggressive) {
    
    // Since we are matching bid we look at asks

    std::vector<Trade> trades;

    for (auto& [passive_price, passive_orders] : asks) {
        // Iterate over all passive order starting from lowest price

        if (passive_price > aggressive.price) {
            // If lowest price is not good enough break
            break;
        }

        while (passive_orders.size() && aggressive.volume) {

            Order& passive = passive_orders.front();

            uint32_t fill_amount = std::min(aggressive.volume, passive.volume);

            aggressive.volume -= fill_amount;
            passive.volume -= fill_amount;

            trades.push_back({
                aggressive.id,  // buy id
                passive.id,     // sell id
                passive.price,  // passive price
                fill_amount     // quantity
            });

            if (!passive.volume) {
                order_locations.erase(passive.id);
                passive_orders.pop_front();
            }
        }

        if (!aggressive.volume) {
            break;
        }

    }

    // If there is remaining volume, add to bids 
    if (aggressive.volume) {
        order_locations[aggressive.id] = {aggressive.side, aggressive.price};
        bids[aggressive.price].push_back(aggressive);
    }

    return trades;
}

std::vector<Trade> Orderbook::match_ask(Order aggressive) {
    
    // Since we are matching ask we look at bids 

    std::vector<Trade> trades;

    for (auto& [passive_price, passive_orders] : bids) {
        // Iterate over all passive order starting from lowest price

        if (passive_price < aggressive.price) {
            // If lowest price is not good enough break
            break;
        }

        while (passive_orders.size() && aggressive.volume) {

            Order& passive = passive_orders.front();

            uint32_t fill_amount = std::min(aggressive.volume, passive.volume);

            aggressive.volume -= fill_amount;
            passive.volume -= fill_amount;

            trades.push_back({
                passive.id,     // buy id
                aggressive.id,  // sell id
                passive.price,  // passive price
                fill_amount     // quantity
            });

            if (!passive.volume) {
                order_locations.erase(passive.id);
                passive_orders.pop_front();
            }
        }

        if (!aggressive.volume) {
            break;
        }

    }

    // If there is remaining volume, add to bids 
    if (aggressive.volume) {
        order_locations[aggressive.id] = {aggressive.side, aggressive.price};
        asks[aggressive.price].push_back(aggressive);
    }

    return trades;
}
