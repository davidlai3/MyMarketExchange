#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    while (true) {
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) break;
        std::cout << "Client: " << buffer << std::endl;
        send(client_socket, buffer, strlen(buffer), 0);  // echo
        memset(buffer, 0, sizeof(buffer));
    }
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::vector<std::thread> threads;

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int res = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (res < 0) {
        std::cerr << "Bind failed\n";
        return -1;
    }
    listen(server_fd, MAX_CLIENTS);

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        std::cout << "Accepted a new client\n";
        threads.emplace_back(std::thread(handle_client, client_socket));
        threads.back().detach(); // Or join later if you want to wait for them
    }

    close(server_fd);
    return 0;
}

