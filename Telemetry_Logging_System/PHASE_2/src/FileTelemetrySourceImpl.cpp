// FileTelemetrySourceImpl.cpp
#include "FileTelemetrySourceImpl.hpp"
#include <fcntl.h>

// Constructor - initialize SafeFile in member initializer list
FileTelemetrySourceImpl::FileTelemetrySourceImpl(const std::string& path)
    : file(path, O_RDONLY), filepath(path)
{
}

// Check if source is open
bool FileTelemetrySourceImpl::openSource() {
    return file.isValid();
}

// Read from source
bool FileTelemetrySourceImpl::readSource(std::string& out) {
    if (!file.isValid()) {
        return false;
    }
    
    char buffer[1024];
    ssize_t bytesRead = file.read(buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0) {
        return false;  // EOF or error
    }
    
    // Null-terminate and assign
    buffer[bytesRead] = '\0';
    out = buffer;
    
    return true;
}