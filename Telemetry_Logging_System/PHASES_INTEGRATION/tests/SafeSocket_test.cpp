#include <gtest/gtest.h>
#include "SafeSocket.hpp"
#include <thread>
#include <chrono>
#include <cstring>

class SafeSocketTest : public ::testing::Test {
protected:
    const std::string socketPath = "/tmp/test_socket.sock";
    
    void SetUp() override {
        // Clean up before each test
        unlink(socketPath.c_str());
    }
    
    void TearDown() override {
        // Clean up after each test
        unlink(socketPath.c_str());
    }
};

// Test 1: Create socket successfully
TEST_F(SafeSocketTest, CreateSocketWorks) {
    SafeSocket socket;
    EXPECT_TRUE(socket.createSocket());
    EXPECT_TRUE(socket.isValid());
}

// Test 2: Bind socket successfully
TEST_F(SafeSocketTest, BindWorks) {
    SafeSocket socket;
    ASSERT_TRUE(socket.createSocket());
    
    EXPECT_TRUE(socket.bind(socketPath));
}

// Test 3: Listen works after bind
TEST_F(SafeSocketTest, ListenWorks) {
    SafeSocket socket;
    ASSERT_TRUE(socket.createSocket());
    ASSERT_TRUE(socket.bind(socketPath));
    
    EXPECT_TRUE(socket.listen());
}

// Test 4: Client-Server communication
TEST_F(SafeSocketTest, ClientServerCommunication) {
    bool serverDone = false;
    std::string receivedMessage;
    
    // Server thread
    std::thread serverThread([&]() {
        SafeSocket server;
        server.createSocket();
        server.bind(socketPath);
        server.listen();
        
        SafeSocket client = server.accept();
        EXPECT_TRUE(client.isValid());
        
        char buffer[1024];
        ssize_t bytesRead = client.read(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            receivedMessage = buffer;
        }
        
        serverDone = true;
    });
    
    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Client thread
    std::thread clientThread([&]() {
        SafeSocket client;
        client.createSocket();
        
        // Try to connect (with retry)
        bool connected = false;
        for (int i = 0; i < 10 && !connected; i++) {
            connected = client.connect(socketPath);
            if (!connected) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        
        EXPECT_TRUE(connected);
        
        if (connected) {
            const char* message = "Hello from client!";
            client.write(message, strlen(message));
        }
    });
    
    serverThread.join();
    clientThread.join();
    
    EXPECT_TRUE(serverDone);
    EXPECT_EQ(receivedMessage, "Hello from client!");
}

// Test 5: Move constructor works
TEST_F(SafeSocketTest, MoveConstructorWorks) {
    SafeSocket socket1;
    socket1.createSocket();
    ASSERT_TRUE(socket1.isValid());
    
    SafeSocket socket2 = std::move(socket1);
    
    EXPECT_FALSE(socket1.isValid());
    EXPECT_TRUE(socket2.isValid());
}

// Test 6: Move assignment works
TEST_F(SafeSocketTest, MoveAssignmentWorks) {
    SafeSocket socket1;
    socket1.createSocket();
    
    SafeSocket socket2;
    socket2.createSocket();
    
    ASSERT_TRUE(socket1.isValid());
    ASSERT_TRUE(socket2.isValid());
    
    socket2 = std::move(socket1);
    
    EXPECT_FALSE(socket1.isValid());
    EXPECT_TRUE(socket2.isValid());
}

// Test 7: Accept returns valid socket
TEST_F(SafeSocketTest, AcceptReturnsValidSocket) {
    SafeSocket server;
    server.createSocket();
    server.bind(socketPath);
    server.listen();
    
    std::thread clientThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SafeSocket client;
        client.createSocket();
        client.connect(socketPath);
    });
    
    SafeSocket acceptedClient = server.accept();
    EXPECT_TRUE(acceptedClient.isValid());
    
    clientThread.join();
}

// Test 8: Destructor cleans up socket file
TEST_F(SafeSocketTest, DestructorCleansUpSocketFile) {
    {
        SafeSocket socket;
        socket.createSocket();
        socket.bind(socketPath);
        
        // Socket file should exist
        EXPECT_EQ(access(socketPath.c_str(), F_OK), 0);
    }  // Destructor called here
    
    // Socket file should be removed
    EXPECT_NE(access(socketPath.c_str(), F_OK), 0);
}

// Test 9: Invalid socket operations fail gracefully
TEST_F(SafeSocketTest, InvalidOperationsFail) {
    SafeSocket socket;  // Not created
    
    EXPECT_FALSE(socket.isValid());
    EXPECT_FALSE(socket.bind(socketPath));
    EXPECT_FALSE(socket.listen());
    
    char buffer[10];
    EXPECT_EQ(socket.read(buffer, sizeof(buffer)), -1);
    EXPECT_EQ(socket.write("test", 4), -1);
}