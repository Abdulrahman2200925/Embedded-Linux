#ifndef SafeSocket_HPP
#define SafeSocket_HPP

#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

class SafeSocket {
private:
    int sockfd;                  // Socket file descriptor
    std::string socketPath;      // Unix domain socket path
    bool isServerSocket;         // Is this a server or client socket?
    
public:
    // Constructor - creates uninitialized socket
    SafeSocket();
    
    // Destructor - closes socket and cleans up
    ~SafeSocket();
    
    // Delete copy operations (Rule of 5)
    SafeSocket(const SafeSocket&) = delete;
    SafeSocket& operator=(const SafeSocket&) = delete;
    
    // Move operations (Rule of 5)
    SafeSocket(SafeSocket&& other) noexcept;
    SafeSocket& operator=(SafeSocket&& other) noexcept;
    
    // Socket creation
    bool createSocket();
    
    // Server operations
    bool bind(const std::string& path);
    bool listen(int backlog = 5);
    SafeSocket accept();  // Returns new socket for client connection
    
    // Client operations
    bool connect(const std::string& path);
    
    // I/O operations
    ssize_t read(void* buffer, size_t count);
    ssize_t write(const void* buffer, size_t count);
    
    // Utility
    bool isValid() const;
    int getFd() const;
    
private:
    // Helper: cleanup socket file
    void cleanup();
};

#endif