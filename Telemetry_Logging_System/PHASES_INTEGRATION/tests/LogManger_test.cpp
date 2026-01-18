

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "LogManager.hpp"
#include "MockSink.hpp"

TEST(LogManagerTest, FlushCallsWriteOnSink) {
    // Step 1: Create LogManager
    LogManager manager;
    
    // Step 2: Create MockSink as shared_ptr
    auto mockSink = std::make_shared<MockSink>();

    // Step 3: Add mock sink to manager
    manager.addSink(mockSink);
    // Step 4: Create a LogMessage
    LogMessage msg("TestApp", "Test", Severity::INFO, "Test message");
    // Step 5: Log the message
    manager.log(msg);
    // Step 6: Set expectation (EXPECT_CALL)
    EXPECT_CALL(*mockSink, write(testing::_)).Times(1);
    // Step 7: Flush
    manager.flush();
}