#ifndef SocketSinkImpl_HPP
#define SocketSinkImpl_HPP

#include "ILogSink.hpp"
#include "SafeSocket.hpp"
#include <string>

class SocketSinkImpl : public ILogSink {
private:
    SafeSocket socket;
    std::string socketPath;
    bool connected;
    
public:
    // Constructor - takes socket path
    explicit SocketSinkImpl(const std::string& path);
    
    // Destructor
    ~SocketSinkImpl() override = default;
    
    // ILogSink interface
    void write(const LogMessage& message) override;
    
private:
    // Helper: connect to server if not connected
    bool ensureConnected();
    
    // Helper: format message for socket transmission
    std::string formatMessage(const LogMessage& message);
};

#endif