// FileTelemetrySourceImpl.cpp
#include "FileTelemetrySourceImpl.hpp"
#include <fcntl.h>
#include <unistd.h>
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

    // ← Seek back to beginning before every read
    lseek(file.getFd(), 0, SEEK_SET);

    char buffer[1024];
    ssize_t bytesRead = file.read(buffer, sizeof(buffer) - 1);

    if (bytesRead <= 0) {
        return false;
    }

    buffer[bytesRead] = '\0';
    out = buffer;

    return true;
}