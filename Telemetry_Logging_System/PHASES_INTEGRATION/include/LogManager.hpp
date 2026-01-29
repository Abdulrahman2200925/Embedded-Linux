#ifndef LogManager_HPP
#define LogManager_HPP

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include "ILogSink.hpp"
#include "RingBuffer.hpp"
#include "ThreadPool.hpp"  
#include <memory>

class LogManager {
private:
    RingBuffer<LogMessage> buffer;
    std::vector<std::shared_ptr<ILogSink>> sinks;
    
    // Threading components
    std::thread consumerThread;
    ThreadPool threadPool;           // For parallel sink writing
    std::atomic<bool> running;       // Controls consumer thread
    mutable std::mutex sinksMutex;   // Protects sinks vector
    
    // Consumer thread function
    void consumerLoop();

public:
    // Constructor - needs buffer size and thread pool size
    explicit LogManager(size_t bufferSize = 100, size_t threadPoolSize = 4);
    
    // Destructor - must stop thread
    ~LogManager();
    
    // Delete copy/move (contains thread)
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;
    
    // Core functionality
    void addSink(std::shared_ptr<ILogSink> sink);
    void log(const LogMessage& message);
    
    // Thread control
    void start();   // Start consumer thread
    void stop();    // Stop consumer thread and drain buffer
};

#endif