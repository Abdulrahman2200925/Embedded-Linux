#ifndef TIMESTAMPED_TEMPERATURE_LOGGER_HPP
#define TIMESTAMPED_TEMPERATURE_LOGGER_HPP

#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

class TemperatureLogger {
private:
    std::string path;
    std::ofstream file;

    // Generate timestamp: "YYYY-MM-DD HH:MM:SS"
    std::string timestamp() {
        using clock = std::chrono::system_clock;

        auto now = clock::now();
        std::time_t t = clock::to_time_t(now);

        std::tm buf{};
        localtime_r(&t, &buf);  // thread-safe on Linux

        std::ostringstream ss;
        ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:

    explicit TemperatureLogger(const std::string &logpath = "temperature.log")
        : path(logpath)
    {
        file.open(path, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Cannot open log file: " << path << "\n";
        }
    }

    ~TemperatureLogger() {
        if (file.is_open())
            file.close();
    }

    // Writes "timestamp + reading" to file + prints it
    void log_line(const std::string &line) {
        std::string out = timestamp() + "  " + line;

        if (file.is_open()) {
            file << out << "\n";
            file.flush();
        }

        std::cout << out << "\n";
    }
};

#endif // TIMESTAMPED_TEMPERATURE_LOGGER_HPP
