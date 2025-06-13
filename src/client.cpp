#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    int res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (res < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::string msg;
    while (true) {
        std::cout << "Message to send: ";
        std::getline(std::cin, msg);
        if (msg == "exit") break;

        send(sock, msg.c_str(), msg.size(), 0);
        int valread = read(sock, buffer, 1024);
        if (valread < 0) {
            std::cerr << "Read error" << std::endl;
            break;
        }
        std::cout << "Server echoed: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
    return 0;
}
