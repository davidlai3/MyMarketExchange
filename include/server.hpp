#pragma once

#include "tradingengine.hpp"

#include <thread>
#include <atomic>
#include <vector>

#define MAX_CLIENTS 10

class TradingServer {
    public:
        TradingServer(int port, TradingEngine &engine);
        void start();
    private:
        void handle_client(int client_socket);

        int server_socket;
        std::atomic<bool> running;
        std::vector<std::thread> client_threads;
        TradingEngine &engine;
};

