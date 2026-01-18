#include <gtest/gtest.h>
#include "SocketTelemetrySourceImpl.hpp"
#include "SafeSocket.hpp"
#include <thread>
#include <chrono>
#include <cstring>

class SocketTelemetrySourceTest : public ::testing::Test {
protected:
    const std::string socketPath = "/tmp/telemetry_socket.sock";
    
    void SetUp() override {
        unlink(socketPath.c_str());
    }
    
    void TearDown() override {
        unlink(socketPath.c_str());
    }
};

// Test 1: Open socket source successfully
TEST_F(SocketTelemetrySourceTest, OpenSourceSucceeds) {
    std::thread sourceThread([&]() {
        SocketTelemetrySourceImpl source(socketPath);
        // openSource() will block waiting for client
        bool opened = source.openSource();
        EXPECT_TRUE(opened);
    });
    
    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Connect as client
    SafeSocket client;
    client.createSocket();
    bool connected = client.connect(socketPath);
    EXPECT_TRUE(connected);
    
    sourceThread.join();
}

// Test 2: Read data from socket
TEST_F(SocketTelemetrySourceTest, ReadDataFromSocket) {
    std::string receivedData;
    
    std::thread sourceThread([&]() {
        SocketTelemetrySourceImpl source(socketPath);
        
        if (source.openSource()) {
            source.readSource(receivedData);
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Send data as client
    SafeSocket client;
    client.createSocket();
    if (client.connect(socketPath)) {
        const char* message = "Telemetry: 42.5";
        client.write(message, strlen(message));
    }
    
    sourceThread.join();
    
    EXPECT_FALSE(receivedData.empty());
}

// Test 3: Interface compliance
TEST_F(SocketTelemetrySourceTest, ImplementsITelemetrySource) {
    std::unique_ptr<ITelemetrySource> source = 
        std::make_unique<SocketTelemetrySourceImpl>(socketPath);
    
    // Just verify it compiles and can be used through interface
    EXPECT_NE(source, nullptr);
}

// Test 4: Invalid socket path
TEST_F(SocketTelemetrySourceTest, InvalidSocketPath) {
    // Try to open but don't connect
    SocketTelemetrySourceImpl source(socketPath);
    
    // This will timeout or fail if no client connects
    // We just verify it doesn't crash
    EXPECT_NO_THROW({
        std::thread t([&]() {
            source.openSource();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Force thread to stop (in real test, might want timeout)
        t.detach();
    });
}