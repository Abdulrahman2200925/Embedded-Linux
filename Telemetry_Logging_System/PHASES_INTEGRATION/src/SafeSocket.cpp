#include "SafeSocket.hpp"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Constructor
SafeSocket::SafeSocket() 
    : sockfd(-1), socketPath(""), isServerSocket(false)
{
}

// Destructor
SafeSocket::~SafeSocket() {
    cleanup();
}

// Move constructor
SafeSocket::SafeSocket(SafeSocket&& other) noexcept
    : sockfd(other.sockfd)
    , socketPath(std::move(other.socketPath))
    , isServerSocket(other.isServerSocket)
{
    other.sockfd = -1;
    other.isServerSocket = false;
}

// Move assignment
SafeSocket& SafeSocket::operator=(SafeSocket&& other) noexcept {
    if (this != &other) {
        // Clean up current socket
        cleanup();
        
        // Steal from other
        sockfd = other.sockfd;
        socketPath = std::move(other.socketPath);
        isServerSocket = other.isServerSocket;
        
        // Invalidate source
        other.sockfd = -1;
        other.isServerSocket = false;
    }
    return *this;
}

// Create socket
bool SafeSocket::createSocket() {
    sockfd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    return sockfd != -1;
}

// Bind (server)
bool SafeSocket::bind(const std::string& path) {
    if (sockfd == -1) return false;
    
    socketPath = path;
    isServerSocket = true;
    
    // Remove old socket file if exists
    ::unlink(path.c_str());
    
    // Setup address structure
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
    
    // Bind socket to path
    int result = ::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    return result != -1;
}

// Listen (server)
bool SafeSocket::listen(int backlog) {
    if (sockfd == -1) return false;
    
    int result = ::listen(sockfd, backlog);
    return result != -1;
}

// Accept (server) - returns new socket for client
SafeSocket SafeSocket::accept() {
    if (sockfd == -1) {
        return SafeSocket();  // Return invalid socket
    }
    
    int clientfd = ::accept(sockfd, nullptr, nullptr);
    
    SafeSocket clientSocket;
    clientSocket.sockfd = clientfd;
    clientSocket.isServerSocket = false;
    
    return clientSocket;  // Move semantics
}

// Connect (client)
bool SafeSocket::connect(const std::string& path) {
    if (sockfd == -1) return false;
    
    socketPath = path;
    
    // Setup address structure
    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
    
    // Connect to server
    int result = ::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    return result != -1;
}

// Read from socket
ssize_t SafeSocket::read(void* buffer, size_t count) {
    if (sockfd == -1) return -1;
    return ::read(sockfd, buffer, count);
}

// Write to socket
ssize_t SafeSocket::write(const void* buffer, size_t count) {
    if (sockfd == -1) return -1;
    return ::write(sockfd, buffer, count);
}

// Check if valid
bool SafeSocket::isValid() const {
    return sockfd != -1;
}

// Get file descriptor
int SafeSocket::getFd() const {
    return sockfd;
}

// Cleanup helper
void SafeSocket::cleanup() {
    if (sockfd != -1) {
        ::close(sockfd);
        
        // If server socket, remove socket file
        if (isServerSocket && !socketPath.empty()) {
            ::unlink(socketPath.c_str());
        }
        
        sockfd = -1;
    }
}