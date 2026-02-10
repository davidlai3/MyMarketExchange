#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "Types.h"
#include "Orderbook.h"

#include <mutex>
#include <memory>

// Exchange listener interface, used for logging
class ExchangeListener {
public:
    virtual ~ExchangeListener() = default;

    virtual void on_order_submitted(const Order& order) = 0;
    virtual void on_order_canceled(uint32_t order_id) = 0;
    virtual void on_trade(const Trade& trade) = 0;
};

class Exchange {
public:

    // Lifecycle

    // Singleton pattern -- only one exchange exists
    static Exchange& instance();
    // Set up orderbook
    void init();
    // Evict all users
    void shutdown();

    // Order handling (MUST BE THREAD SAFE)

    bool submit_order(const Order& o);
    bool cancel_order(uint32_t order_id);

    // Listener management

    void add_listener(std::shared_ptr<ExchangeListener> listener);
    void remove_listener(std::shared_ptr<ExchangeListener> listener);


private:
    Exchange() = default;
    Exchange(const Exchange&) = delete;
    Exchange& operator=(const Exchange&) = delete;

    Orderbook book;
    std::mutex mtx;

    void notify_order_submitted(const Order& order);
    void notify_order_canceled(uint32_t order_id);
    void notify_trade(const Trade& trade);

    std::vector<std::shared_ptr<ExchangeListener>> listeners_;
};

#endif
