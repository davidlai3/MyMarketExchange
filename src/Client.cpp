#include "../include/Client.h"
#include "../include/Types.h"
#include "../include/OrderProtocol.h"
#include "../include/Debug.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Client::Client(uint16_t port) : 
    connect_fd(-1), server_port(port) {
    setup_socket();
}

Client::~Client() {
    if (connect_fd >= 0) {
        close(connect_fd);
    }
}

void Client::setup_socket() {
    // Create socket
    connect_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Specify server address
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Send connection request
    int ret = connect(connect_fd, (struct sockaddr*) &server, sizeof(server));

    if (ret == -1) {
        std::cout << "Connection error" << std::endl;
        return;
    }
}

void Client::run() {
    debug("[Client]: Sending order");
    // Sending data
    Order o{Side::BUY, 100, 10};
    OrderProtocol::send_order(connect_fd, o);
}
