#ifndef CLIENT_H
#define CLIENT_H

#include "Exchange.h"
#include <cstdint>

class Client {
public:
    explicit Client(uint16_t port);
    ~Client();

    void run();

private:
    int connect_fd;
    uint16_t server_port;
    void setup_socket();
};

#endif
