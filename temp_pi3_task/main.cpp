#include <iostream>                     // <-- REQUIRED for std::cerr, std::cout
#include "UART.hpp"
#include "TimestampedTemperatureLogger.hpp"

int main() {
    UART uart("/dev/serial0", 115200);
    TemperatureLogger logger("temperature.log");

    if (!uart.ok()) {
        std::cerr << "UART failed\n";   // now works
        return 1;
    }

    char line[128];

    while (true) {
        int n = uart.read_line(line, sizeof(line));

        if (n > 0) {
            logger.log_line(line);      // now works
        }
    }
}
