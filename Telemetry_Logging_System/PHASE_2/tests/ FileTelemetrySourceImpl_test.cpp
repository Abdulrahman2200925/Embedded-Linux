#include <gtest/gtest.h>
#include "FileTelemetrySourceImpl.hpp"
#include <fstream>
#include <unistd.h>

class FileTelemetrySourceTest : public ::testing::Test {
protected:
    const std::string testFile = "/tmp/telemetry_test.txt";
    
    void SetUp() override {
        unlink(testFile.c_str());
    }
    
    void TearDown() override {
        unlink(testFile.c_str());
    }
    
    void createTestFile(const std::string& content) {
        std::ofstream file(testFile);
        file << content;
        file.close();
    }
};

// Test 1: Open valid file successfully
TEST_F(FileTelemetrySourceTest, OpenSourceSucceeds) {
    createTestFile("test data");
    
    FileTelemetrySourceImpl source(testFile);
    EXPECT_TRUE(source.openSource());
}

// Test 2: Open non-existent file fails
TEST_F(FileTelemetrySourceTest, OpenNonExistentFileFails) {
    FileTelemetrySourceImpl source("/nonexistent/file.txt");
    EXPECT_FALSE(source.openSource());
}

// Test 3: Read single line
TEST_F(FileTelemetrySourceTest, ReadSingleLine) {
    createTestFile("Temperature: 25.5°C\n");
    
    FileTelemetrySourceImpl source(testFile);
    ASSERT_TRUE(source.openSource());
    
    std::string data;
    EXPECT_TRUE(source.readSource(data));
    EXPECT_EQ(data, "Temperature: 25.5°C\n");
}

// Test 4: Read multiple lines
TEST_F(FileTelemetrySourceTest, ReadMultipleLines) {
    createTestFile("Line 1\nLine 2\nLine 3\n");
    
    FileTelemetrySourceImpl source(testFile);
    ASSERT_TRUE(source.openSource());
    
    std::string data;
    int linesRead = 0;
    
    while (source.readSource(data)) {
        linesRead++;
    }
    
    EXPECT_GT(linesRead, 0);
}

// Test 5: Read returns false on EOF
TEST_F(FileTelemetrySourceTest, ReadReturnsFalseOnEOF) {
    createTestFile("Single line");
    
    FileTelemetrySourceImpl source(testFile);
    ASSERT_TRUE(source.openSource());
    
    std::string data;
    EXPECT_TRUE(source.readSource(data));  // First read succeeds
    EXPECT_FALSE(source.readSource(data)); // Second read fails (EOF)
}

// Test 6: Read from closed file fails
TEST_F(FileTelemetrySourceTest, ReadFromClosedFileFails) {
    FileTelemetrySourceImpl source("/nonexistent.txt");
    ASSERT_FALSE(source.openSource());
    
    std::string data;
    EXPECT_FALSE(source.readSource(data));
}

// Test 7: Move semantics work (Rule of Zero)
TEST_F(FileTelemetrySourceTest, MoveSemantics) {
    createTestFile("test data");
    
    FileTelemetrySourceImpl source1(testFile);
    ASSERT_TRUE(source1.openSource());
    
    // Move construct
    FileTelemetrySourceImpl source2 = std::move(source1);
    
    // source2 should work
    std::string data;
    EXPECT_TRUE(source2.readSource(data));
}

// Test 8: Read empty file
TEST_F(FileTelemetrySourceTest, ReadEmptyFile) {
    createTestFile("");
    
    FileTelemetrySourceImpl source(testFile);
    ASSERT_TRUE(source.openSource());
    
    std::string data;
    EXPECT_FALSE(source.readSource(data));
}

// Test 9: Interface compliance
TEST_F(FileTelemetrySourceTest, ImplementsITelemetrySource) {
    createTestFile("test");
    
    // Can be used through interface pointer
    std::unique_ptr<ITelemetrySource> source = 
        std::make_unique<FileTelemetrySourceImpl>(testFile);
    
    EXPECT_TRUE(source->openSource());
    
    std::string data;
    EXPECT_TRUE(source->readSource(data));
}