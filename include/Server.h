#ifndef SERVER_H
#define SERVER_H

#include "Exchange.h"
#include <cstdint>

class Server {
public:
    explicit Server(uint16_t port);
    ~Server();

    void run();   // blocking accept-loop

private:
    int listen_fd;
    uint16_t port;

    void setup_socket();
    void accept_loop();
    void handle_client(int client_fd);

    Exchange& exchange = Exchange::instance();
};

#endif
