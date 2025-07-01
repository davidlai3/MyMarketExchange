#include "../include/server.hpp"

#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

TradingServer::TradingServer(int port, TradingEngine &engine) : engine(engine), running(true) {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::vector<std::thread> threads;

    // Create socket file descriptor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Socket creation failed\n";
        exit(1);
    }
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int res = bind(server_socket, (struct sockaddr *)&address, sizeof(address));
    if (res < 0) {
        std::cerr << "Bind failed\n";
        exit(1);
    }
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen failed\n";
        exit(1);
    }

    std::cout << "Server initialized on " << port << "\n";
};


void TradingServer::start() {
    std::cout << "Listening for clients...\n";
    while (running) {
        int client = accept(server_socket, nullptr, nullptr);
        if (client >= 0) {
            client_threads.emplace_back(&TradingServer::handle_client, this, client);
        }
    }

    for (auto &t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}


void TradingServer::handle_client(int client_socket) {
    while (true) {
        char order_buffer[sizeof(OrderPacket)];
        recv(client_socket, order_buffer, sizeof(OrderPacket), 0);

        OrderPacket pkt = OrderPacket::deserialize(order_buffer);

        engine.submitOrder(pkt.symbol, pkt.side, pkt.price, pkt.quantity);
    }
}
