#include "SafeFile.hpp"
#include <unistd.h>

// Constructor
SafeFile::SafeFile(const std::string& path, int flags)
    : filepath(path), fd(::open(path.c_str(), flags, 0644))
{
    // fd is now initialized (will be -1 if open fails)
}

// Move constructor
SafeFile::SafeFile(SafeFile&& other) noexcept
    : fd(other.fd), filepath(std::move(other.filepath))
{
    other.fd = -1;
}

// Move assignment
SafeFile& SafeFile::operator=(SafeFile&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    
    // Close current file
    if (fd != -1) {
        ::close(fd);
    }
    
    // Steal from other
    fd = other.fd;
    filepath = std::move(other.filepath);
    
    // Invalidate source
    other.fd = -1;
    
    return *this;
}

// Destructor
SafeFile::~SafeFile() {
    if (fd != -1) {
        ::close(fd);
    }
}

// Check validity
bool SafeFile::isValid() const {
    return fd != -1;
}

// Get file descriptor
int SafeFile::getFd() const {
    return fd;
}

// Write
ssize_t SafeFile::write(const void* buffer, size_t count) {
    if (fd == -1) return -1;
    return ::write(fd, buffer, count);
}

// Read
ssize_t SafeFile::read(void* buffer, size_t count) {
    if (fd == -1) return -1;
    return ::read(fd, buffer, count);
}