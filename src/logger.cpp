#include "../include/logger.hpp"

#include <iostream>

/* TODO: If we can have the logger running asynchronously, we can avoid 
 * blocking the main thread. Look into creating a dedicated logging thread. */
Logger::Logger(const std::string &filename) {
    file.open(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
    } else {
        log("Logger initialized.");
    }
}

void Logger::log(const std::string &message) {
    std::lock_guard<std::mutex> lock{mtx};

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    file << "[" << std::ctime(&in_time_t);
    file.seekp(-1, std::ios_base::cur);
    file << "] " << message << "\n";
}
