#include "../include/Server.h"
#include "../include/OrderProtocol.h"
#include "../include/Debug.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <iostream>

Server::Server(uint16_t port_)
    : listen_fd(-1), port(port_) {
    setup_socket();
}

Server::~Server() {
    if (listen_fd >= 0) {
        close(listen_fd);
    }
}

void Server::setup_socket() {
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        std::exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        std::exit(EXIT_FAILURE);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        std::exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 16) < 0) {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }

    std::cout << "[Server]: Listening on port " << port << "\n";
}

void Server::run() {
    accept_loop();
}

void Server::accept_loop() {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // Spin a thread per client
        std::thread t(&Server::handle_client, this, client_fd);
        t.detach();  // or store/join later if you want
    }
}

void Server::handle_client(int client_fd) {
    // Server should indefinitely loop and read from client connection

    debug("[Server]: Spinning up thread for client with fd " << client_fd);

    Order o;

    while (OrderProtocol::recv_order(client_fd, o)) {
        exchange.submit_order(o);
    }

    debug("[Server]: Closing client with fd " << client_fd);

    close(client_fd);
}
