#include "LogManager.hpp"
#include <iostream>

LogManager::LogManager(size_t bufferSize, size_t threadPoolSize)
    : buffer(bufferSize)
    , threadPool(threadPoolSize)
    , running(false)
{
    // Don't start thread in constructor - explicit start() call
}

LogManager::~LogManager() {
    // Ensure thread is stopped
    stop();
}

void LogManager::addSink(std::shared_ptr<ILogSink> sink) {
    std::lock_guard<std::mutex> lock(sinksMutex);
    sinks.push_back(sink);
}

void LogManager::log(const LogMessage& message) {
    // Non-blocking push - if full, message is dropped
    if (!buffer.tryPush(LogMessage(message))) {
        
        std::cerr << "LogManager: Buffer full, message dropped\n";
    }
}

void LogManager::start() {
    if (running.load()) {
        return;  // Already running
    }
    
    running.store(true);
    
    // Launch consumer thread
    consumerThread = std::thread(&LogManager::consumerLoop, this);
}

void LogManager::stop() {
    if (!running.load()) {
        return;  // Not running
    }
    
    // Signal thread to stop
    running.store(false);
    
    // Wake up the consumer thread if it's waiting
    buffer.shutdown();
    
    // Wait for thread to finish
    if (consumerThread.joinable()) {
        consumerThread.join();
    }
    
    // Process remaining messages in buffer
    LogMessage msg("", "", Severity::DEBUG, "");
    while (buffer.tryPop(msg)) {
        // Write remaining messages synchronously
        std::lock_guard<std::mutex> lock(sinksMutex);
        for (auto& sink : sinks) {
            sink->write(msg);
        }
    }
}

void LogManager::consumerLoop() {
    LogMessage msg("", "", Severity::DEBUG, "");
    
    while (running.load()) {
        // Blocking pop - waits until message available
        if (buffer.pop(msg)) {
            // Got a message - write to all sinks using thread pool
            
            // Make a copy of sinks to avoid holding lock during writes
            std::vector<std::shared_ptr<ILogSink>> sinksCopy;
            {
                std::lock_guard<std::mutex> lock(sinksMutex);
                sinksCopy = sinks;
            }
            
            // Submit write tasks to thread pool
            for (auto& sink : sinksCopy) {
                // Capture by value to ensure lifetime
                threadPool.enqueue([sink, msg]() {
                    sink->write(msg);
                });
            }
        }
        // If pop() returns false, it means shutdown was called
    }
    
    // Thread exiting
    std::cout << "LogManager: Consumer thread stopped\n";
}
