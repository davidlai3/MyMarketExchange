#pragma once
#include <fstream>
#include <string>
#include <mutex>

class Logger {
    public:
        Logger(const std::string &filename);
        void log(const std::string &message);
    private:
        std::ofstream file;
        std::mutex mtx;
};

