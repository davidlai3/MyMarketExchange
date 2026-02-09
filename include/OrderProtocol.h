#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>

#include "Types.h"

/*
* Protocol class for serializing and deserializing Orders
* The protocol format is as follows:
* - 4 bytes: order id (uint32_t)
* - 1 byte: side (0 for BUY, 1 for SELL)
* - 4 bytes: price (uint32_t)
* - 4 bytes: volume (uint32_t)
* - 8 bytes: timestamp (uint64_t)
*/

class OrderProtocol {
public:
    static bool send_order(int fd, const Order& o);
    static bool recv_order(int fd, Order& o);

    static bool send_all(int fd, void* data, size_t len);
    static bool recv_all(int fd, void* data, size_t len);
};

#endif
