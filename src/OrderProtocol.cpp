#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

#include "../include/OrderProtocol.h"

static inline uint64_t htonll(uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t lo = static_cast<uint32_t>(x & 0xFFFFFFFFULL);
    uint32_t hi = static_cast<uint32_t>(x >> 32);
    return (static_cast<uint64_t>(htonl(lo)) << 32) |
           static_cast<uint64_t>(htonl(hi));
#else
    return x;
#endif
}

static inline uint64_t ntohll(uint64_t x) {
    return htonll(x);
}

bool OrderProtocol::send_order(int fd, const Order& o) {

    uint32_t id = htonl(o.id);
    uint8_t side = o.side == Side::BUY ? 1 : 0; // Buy is 1 sell is 0
    uint32_t price = htonl(o.price);
    uint32_t volume = htonl(o.volume);
    uint64_t timestamp = htonll(o.timestamp);

    std::vector<uint8_t> buf;
    buf.resize(4 + 1 + 4 + 4 + 8);
    size_t off = 0;

    std::memcpy(buf.data() + off, &id, 4);
    off += 4;
    std::memcpy(buf.data() + off, &side, 1);
    off += 1;
    std::memcpy(buf.data() + off, &price, 4);
    off += 4;
    std::memcpy(buf.data() + off, &volume, 4);
    off += 4;
    std::memcpy(buf.data() + off, &timestamp, 8);

    return send_all(fd, buf.data(), buf.size());
}

bool OrderProtocol::recv_order(int fd, Order& o) {

    constexpr uint32_t ORDER_WIRE_SIZE = 21;
    char data[ORDER_WIRE_SIZE];

    if (!recv_all(fd, data, sizeof(data))) {
        return false;
    }

    uint32_t id;
    uint8_t side;
    uint32_t price;
    uint32_t volume;
    uint64_t timestamp;

    size_t off = 0;

    std::memcpy(&id, data + off, 4);
    off += 4;
    std::memcpy(&side, data + off, 1);
    off += 1;
    std::memcpy(&price, data + off, 4);
    off += 4;
    std::memcpy(&volume, data + off, 4);
    off += 4;
    std::memcpy(&timestamp, data + off, 8);

    o.id = ntohl(id);
    o.side = side == 1 ? Side::BUY : Side::SELL;
    o.price = ntohl(price);
    o.volume = ntohl(volume);
    o.timestamp = ntohll(timestamp);

    return true;
}

bool OrderProtocol::send_all(int fd, void* data, size_t len) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    while (len > 0) {
        ssize_t n = write(fd, p, len);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
        }

        p += n;
        len -= n;
    }

    return true;
}

bool OrderProtocol::recv_all(int fd, void* data, size_t len) {
    uint8_t* p = static_cast<uint8_t*>(data);

    while (len > 0) {
        ssize_t n = read(fd, p, len);

        if (n == 0) {
            // peer closed connection 
            return false;
        }

        if (n < 0) {
            // error
            if (errno == EINTR) {
                continue;
            }
            return false;
        }

        p += n;
        len -= n;
    }

    return true;
}
