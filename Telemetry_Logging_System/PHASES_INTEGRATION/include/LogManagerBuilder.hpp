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
    
public:
    // Constructor - initializes empty builder
    LogManagerBuilder();
    
    // Add a sink - returns *this for chaining
    LogManagerBuilder& addSink(std::shared_ptr<ILogSink> sink);
    
    // Build and return the LogManager
    LogManager build() const;
};

#endif