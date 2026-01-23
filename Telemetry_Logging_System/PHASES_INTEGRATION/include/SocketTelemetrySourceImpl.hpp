#ifndef SocketTelemetrySourceImpl_HPP
#define SocketTelemetrySourceImpl_HPP

#include "ITelemetrySource.hpp"
#include "SafeSocket.hpp"
#include <string>

class SocketTelemetrySourceImpl : public ITelemetrySource {
private:
    SafeSocket serverSocket;
    SafeSocket clientSocket;  // For accepted connection
    std::string socketPath;
    
public:
    SocketTelemetrySourceImpl(const std::string& socketPath);
    
    bool openSource() override;
    bool readSource(std::string& out) override;
};

#endif