#ifndef LogManagerBuilder_HPP
#define LogManagerBuilder_HPP

#include "LogManager.hpp"
#include "ILogSink.hpp"
#include <memory>
#include <vector>

class LogManagerBuilder {
private:
    // Store sinks to be added to LogManager
    std::vector<std::shared_ptr<ILogSink>> sinks;
    size_t bufferSize;        
    size_t threadPoolSize;    
    
public:
    // Constructor - initializes empty builder with defaults
    LogManagerBuilder();
    
    // Set buffer size - 
    LogManagerBuilder& setBufferSize(size_t size);
    
    // Set thread pool size - 
    LogManagerBuilder& setThreadPoolSize(size_t size);
    
    // Add a sink - returns *this for chaining
    LogManagerBuilder& addSink(std::shared_ptr<ILogSink> sink);
    
    // Build and return the LogManager as unique_ptr 
    std::unique_ptr<LogManager> build() const;
};

#endif