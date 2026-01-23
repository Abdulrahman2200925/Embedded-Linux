#include "LogManagerBuilder.hpp"

// Constructor - nothing special needed
LogManagerBuilder::LogManagerBuilder() {
    // sinks vector is automatically initialized as empty
}

// Add a sink to the builder
LogManagerBuilder& LogManagerBuilder::addSink(std::shared_ptr<ILogSink> sink) {
    // Add sink to our temporary collection
    sinks.push_back(sink);
    
    // Return reference to this builder for chaining
    return *this;
}

// Build the LogManager
LogManager LogManagerBuilder::build() const {
    // Step 1: Create a new LogManager
    LogManager logger;
    
    // Step 2: Add all sinks to it
    for (const auto& sink : sinks) {
        logger.addSink(sink);
    }
    
    // Step 3: Return the configured LogManager
    return logger;
}