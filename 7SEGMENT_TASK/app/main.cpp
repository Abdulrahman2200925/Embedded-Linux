#include "../inc/SEGMENT.hpp"
#include "../inc/GPIO.hpp"

#include <csignal>
#include <atomic>

// Global flag controlled by SIGINT
static std::atomic<bool> g_running(true);

// Signal handler for Ctrl+C
void handle_sigint(int)
{
    g_running = false;
}

int main()
{
    // Register Ctrl+C handler
    std::signal(SIGINT, handle_sigint);

    // Create Seven-Segment display (a–g)
    SevenSegment display(
        GPIO(17, GPIO::Direction::Out), // a
        GPIO(18, GPIO::Direction::Out), // b
        GPIO(27, GPIO::Direction::Out), // c
        GPIO(22, GPIO::Direction::Out), // d
        GPIO(23, GPIO::Direction::Out), // e
        GPIO(24, GPIO::Direction::Out), // f
        GPIO(25, GPIO::Direction::Out)  // g
    );

   
    // Clear display on startup in case last run crashed
    display.clear();

    // Main loop
    while (g_running) {
        // Read digit from terminal (0–9)
        int digit = display.readDigit();

        // Output digit to seven-segment display
        display << digit;
    }

    // Graceful shutdown:
    // turn off all segments before exiting
    display.clear();

    return 0;
}
