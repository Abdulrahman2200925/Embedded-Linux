#ifndef TelemetryReader_HPP
#define TelemetryReader_HPP

#include "ITelemetrySource.hpp"
#include "LogManager.hpp"
#include "LogFormatter.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

template<typename Policy>
class TelemetryReader {
private:
    // Components
    ITelemetrySource& source;           // Telemetry data source
    LogManager& logManager;             // Where to send logs
    LogFormatter<Policy> formatter;     // Formats raw data
    
    // Threading
    std::thread readerThread;
    std::atomic<bool> running;
    
    // Configuration
    std::chrono::milliseconds readInterval;
    
    // Thread function
    void readLoop();
    
public:
    // Constructor
    TelemetryReader(ITelemetrySource& source, 
                    LogManager& logManager,
                    std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
    
    // Destructor
    ~TelemetryReader();
    
    // Delete copy and move (contains thread and references)
    TelemetryReader(const TelemetryReader&) = delete;
    TelemetryReader& operator=(const TelemetryReader&) = delete;
    TelemetryReader(TelemetryReader&&) = delete;
    TelemetryReader& operator=(TelemetryReader&&) = delete;
    
    // Control
    void start();
    void stop();
    
    // Query state
    bool isRunning() const;
};

// ============================================================================
// TEMPLATE IMPLEMENTATION (must be in header)
// ============================================================================

template<typename Policy>
TelemetryReader<Policy>::TelemetryReader(
    ITelemetrySource& source,
    LogManager& logManager,
    std::chrono::milliseconds interval)
    : source(source)
    , logManager(logManager)
    , formatter()
    , running(false)
    , readInterval(interval)
{
    // Don't start thread in constructor
}

template<typename Policy>
TelemetryReader<Policy>::~TelemetryReader() {
    // Ensure thread is stopped
    stop();
}

template<typename Policy>
void TelemetryReader<Policy>::start() {
    if (running.load()) {
        std::cerr << "TelemetryReader: Already running\n";
        return;
    }
    
    // Open the source
    if (!source.openSource()) {
        std::cerr << "TelemetryReader: Failed to open source\n";
        return;
    }
    
    // Start the thread
    running.store(true);
    readerThread = std::thread(&TelemetryReader::readLoop, this);
    
    std::cout << "TelemetryReader: Started\n";
}

template<typename Policy>
void TelemetryReader<Policy>::stop() {
    if (!running.load()) {
        return;  // Not running
    }
    
    // Signal thread to stop
    running.store(false);
    
    // Wait for thread to finish
    if (readerThread.joinable()) {
        readerThread.join();
    }
    
    std::cout << "TelemetryReader: Stopped\n";
}

template<typename Policy>
bool TelemetryReader<Policy>::isRunning() const {
    return running.load();
}

template<typename Policy>
void TelemetryReader<Policy>::readLoop() {
    std::string rawData;
    
    while (running.load()) {
        // Read from source
        if (source.readSource(rawData)) {
            // Format raw data to LogMessage using Policy
            auto optionalMsg = formatter.formatDataToLogMsg(rawData);
            
            if (optionalMsg.has_value()) {
                // Successfully formatted - log it
                logManager.log(optionalMsg.value());
            } else {
                // Failed to parse/format
                std::cerr << "TelemetryReader: Failed to format data: " 
                          << rawData << "\n";
            }
        } else {
            // Failed to read from source
            std::cerr << "TelemetryReader: Failed to read from source\n";
            
           
        }
        
        // Sleep before next read
        std::this_thread::sleep_for(readInterval);
    }
    
    std::cout << "TelemetryReader: Read loop exited\n";
}

#endif