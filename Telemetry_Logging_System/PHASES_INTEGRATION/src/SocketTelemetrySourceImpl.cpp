#include "SocketTelemetrySourceImpl.hpp"

SocketTelemetrySourceImpl::SocketTelemetrySourceImpl(const std::string& path)
    : socketPath(path)
{
}

bool SocketTelemetrySourceImpl::openSource() {
    // Create server socket
    if (!serverSocket.createSocket()) return false;
    
    // Bind to path
    if (!serverSocket.bind(socketPath)) return false;
    
    // Listen for connections
    if (!serverSocket.listen()) return false;
    
    // Accept one client
    clientSocket = serverSocket.accept();  // Move semantics!
    
    return clientSocket.isValid();
}

bool SocketTelemetrySourceImpl::readSource(std::string& out) {
    if (!clientSocket.isValid()) return false;
    
    char buffer[1024];
    ssize_t bytesRead = clientSocket.read(buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0) return false;
    
    buffer[bytesRead] = '\0';
    out = buffer;
    
    return true;
}