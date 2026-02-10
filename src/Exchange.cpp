#include "../include/Exchange.h"
#include "../include/Debug.h"

Exchange& Exchange::instance() {
    static Exchange ex;
    return ex;
}

void Exchange::init() {

}

void Exchange::shutdown() {

}

// Order handling

bool Exchange::submit_order(const Order& o) {

    debug("[Exchange]: " << (o.side == Side::BUY ? "BUY" : "SELL") << " for " 
            << o.price << " at " << o.volume);

    std::lock_guard<std::mutex> lock(mtx);

    std::vector<Trade> trades = book.add_order(o);

    return true;
}

bool Exchange::cancel_order(uint32_t order_id) {

    std::lock_guard<std::mutex> lock(mtx);

    return book.cancel_order(order_id);

}
