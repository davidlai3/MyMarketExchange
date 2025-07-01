#include "../include/order.hpp"

#include <arpa/inet.h>

uint64_t htond(double value) {
    uint64_t temp;
    memcpy(&temp, &value, sizeof(temp));
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        temp = ((uint64_t)htonl(temp & 0xFFFFFFFF) << 32) | htonl(temp >> 32);
#endif
    return temp;
}

double ntohd(uint64_t value) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    value = ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) | ntohl(value >> 32);
#endif
    double temp;
    memcpy(&temp, &value, sizeof(temp));
    return temp;
}

void OrderPacket::serialize(OrderPacket &packet, char* buffer) const {
    memcpy(buffer, packet.symbol, 16);
    buffer[16] = static_cast<int>(packet.side);

    uint64_t price_net = htond(packet.price);
    memcpy(buffer + 17, &price_net, sizeof(price_net));

    int32_t quantity_net = htonl(packet.quantity);
    memcpy(buffer + 25, &quantity_net, sizeof(quantity_net));
}

OrderPacket OrderPacket::deserialize(const char* buffer) {
    OrderPacket packet;
    memcpy(packet.symbol, buffer, 16);
    packet.symbol[15] = '\0';

    packet.side = static_cast<Side>(static_cast<uint8_t>(buffer[16]));

    uint64_t price_net;
    memcpy(&price_net, buffer + 17, sizeof(price_net));
    packet.price = ntohd(price_net);

    int32_t quantity_net;
    memcpy(&quantity_net, buffer + 25, sizeof(quantity_net));
    packet.quantity = ntohd(quantity_net);

}
