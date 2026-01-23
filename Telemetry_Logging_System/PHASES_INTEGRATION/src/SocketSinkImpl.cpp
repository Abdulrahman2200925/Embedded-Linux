#include "SocketSinkImpl.hpp"
#include <iostream>
#include <sstream>

// Constructor
SocketSinkImpl::SocketSinkImpl(const std::string& path)
    : socketPath(path)
    , connected(false)
{
    // Don't connect in constructor - wait until first write
    // This allows the sink to be created before the server starts
}

// Write method - implements ILogSink interface
void SocketSinkImpl::write(const LogMessage& message) {
    // Ensure we're connected
    if (!ensureConnected()) {
        std::cerr << "SocketSink: Failed to connect to " << socketPath << "\n";
        return;
    }
    
    // Format the message
    std::string formattedMsg = formatMessage(message);
    
    // Send to socket
    ssize_t bytesWritten = socket.write(formattedMsg.c_str(), formattedMsg.length());
    
    if (bytesWritten < 0) {
        std::cerr << "SocketSink: Failed to write to socket\n";
        connected = false;  // Mark as disconnected for retry
    }
}

// Helper: Ensure socket is connected
bool SocketSinkImpl::ensureConnected() {
    if (connected && socket.isValid()) {
        return true;  // Already connected
    }
    
    // Create socket
    if (!socket.createSocket()) {
        return false;
    }
    
    // Connect to server
    if (!socket.connect(socketPath)) {
        return false;
    }
    
    connected = true;
    return true;
}

// Helper: Format message for transmission
std::string SocketSinkImpl::formatMessage(const LogMessage& message) {
    std::ostringstream oss;
    
    // Use the same format as operator
    oss << "[" << message.getAppName() << "],"
        << "[" << message.getTimeStamp() << "],"
        << "[" << message.getContext() << "],"
        << "[" << severityToString(message.getSeverity()) << "],"
        << "[" << message.getText() << "]";
    
    // Add newline for easier parsing on receiver side
    oss << "\n";
    
    return oss.str();
}