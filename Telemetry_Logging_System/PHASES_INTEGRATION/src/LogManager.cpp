#include "LogManager.hpp"

LogManager::LogManager(size_t bufferSize)
    : buffer(bufferSize)  // Create ring buffer with capacity
{
}

void LogManager::addSink(std::shared_ptr<ILogSink> sink) {
    sinks.push_back(sink);
}

void LogManager::log(const LogMessage& message) {
    // Try to push into ring buffer
    if (!buffer.tryPush(LogMessage(message))) {
        // Buffer full - auto-flush and try again
        flush();
        buffer.tryPush(LogMessage(message));
    }
}

void LogManager::flush() {
    LogMessage msg("", "", Severity::DEBUG, "");  // Temporary
    
    // Pop all messages from buffer
    while (buffer.tryPop(msg)) {
        // Write to all sinks
        for (auto& sink : sinks) {
            sink->write(msg);
        }
    }
}