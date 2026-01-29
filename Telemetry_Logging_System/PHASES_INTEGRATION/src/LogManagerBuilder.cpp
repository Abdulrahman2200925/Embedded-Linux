#include "LogManagerBuilder.hpp"

// Constructor - initialize with defaults
LogManagerBuilder::LogManagerBuilder() 
    : bufferSize(100)
    , threadPoolSize(4)
{
    // sinks vector is automatically initialized as empty
}

// Set buffer size
LogManagerBuilder& LogManagerBuilder::setBufferSize(size_t size) {
    bufferSize = size;
    return *this;
}

// Set thread pool size
LogManagerBuilder& LogManagerBuilder::setThreadPoolSize(size_t size) {
    threadPoolSize = size;
    return *this;
}

// Add a sink to the builder
LogManagerBuilder& LogManagerBuilder::addSink(std::shared_ptr<ILogSink> sink) {
    // Add sink to our temporary collection
    sinks.push_back(sink);
    
    // Return reference to this builder for chaining
    return *this;
}

// Build the LogManager - return as unique_ptr
std::unique_ptr<LogManager> LogManagerBuilder::build() const {
    // Step 1: Create a new LogManager on the heap
    auto logger = std::make_unique<LogManager>(bufferSize, threadPoolSize);
    
    // Step 2: Add all sinks to it
    for (const auto& sink : sinks) {
        logger->addSink(sink);
    }
    
    // Step 3: Return the unique_ptr (ownership transferred)
    return logger;
}