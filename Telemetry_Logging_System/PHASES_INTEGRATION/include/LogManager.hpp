#ifndef LogManager_HPP
#define LogManager_HPP

#include <vector>
#include "ILogSink.hpp"
#include "RingBuffer.hpp"  // ← Include ring buffer
#include <memory>

class LogManager {
private:

    RingBuffer<LogMessage> buffer;
    
    std::vector<std::shared_ptr<ILogSink>> sinks;

public:
    // Constructor now needs buffer size
    explicit LogManager(size_t bufferSize = 100);
    
    void addSink(std::shared_ptr<ILogSink> sink);
    void log(const LogMessage& message);
    void flush();
};

#endif