#include <gtest/gtest.h>
#include "SafeFile.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <cstring>

class SafeFileTest : public ::testing::Test {
protected:
    const std::string testFile = "/tmp/safefile_test.txt";
    
    void SetUp() override {
        // Clean up before each test
        unlink(testFile.c_str());
    }
    
    void TearDown() override {
        // Clean up after each test
        unlink(testFile.c_str());
    }
};

// Test 1: Constructor opens file successfully
TEST_F(SafeFileTest, ConstructorOpensFile) {
    SafeFile file(testFile, O_WRONLY | O_CREAT | O_TRUNC);
    EXPECT_TRUE(file.isValid());
}

// Test 2: Invalid file path fails gracefully
TEST_F(SafeFileTest, InvalidPathFails) {
    SafeFile file("/invalid/path/file.txt", O_RDONLY);
    EXPECT_FALSE(file.isValid());
}

// Test 3: Write operation works
TEST_F(SafeFileTest, WriteWorks) {
    SafeFile file(testFile, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_TRUE(file.isValid());
    
    const char* data = "Hello RAII!";
    ssize_t written = file.write(data, strlen(data));
    
    EXPECT_EQ(written, static_cast<ssize_t>(strlen(data)));
}

// Test 4: Read operation works
TEST_F(SafeFileTest, ReadWorks) {
    // First write data
    {
        SafeFile file(testFile, O_WRONLY | O_CREAT | O_TRUNC);
        const char* data = "Test Data";
        file.write(data, strlen(data));
    }
    
    // Then read it back
    SafeFile file(testFile, O_RDONLY);
    ASSERT_TRUE(file.isValid());
    
    char buffer[100];
    ssize_t bytesRead = file.read(buffer, sizeof(buffer));
    
    EXPECT_GT(bytesRead, 0);
    buffer[bytesRead] = '\0';
    EXPECT_STREQ(buffer, "Test Data");
}

// Test 5: Move constructor works
TEST_F(SafeFileTest, MoveConstructorWorks) {
    SafeFile file1(testFile, O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_TRUE(file1.isValid());
    
    // Move file1 to file2
    SafeFile file2 = std::move(file1);
    
    // file1 should be invalid
    EXPECT_FALSE(file1.isValid());
    
    // file2 should be valid
    EXPECT_TRUE(file2.isValid());
}

// Test 6: Move assignment works
TEST_F(SafeFileTest, MoveAssignmentWorks) {
    SafeFile file1(testFile, O_WRONLY | O_CREAT | O_TRUNC);
    SafeFile file2("/tmp/other.txt", O_WRONLY | O_CREAT | O_TRUNC);
    
    ASSERT_TRUE(file1.isValid());
    ASSERT_TRUE(file2.isValid());
    
    // Move assign
    file2 = std::move(file1);
    
    EXPECT_FALSE(file1.isValid());
    EXPECT_TRUE(file2.isValid());
    
    // Clean up
    unlink("/tmp/other.txt");
}

// Test 7: Destructor closes file (RAII)
TEST_F(SafeFileTest, DestructorClosesFile) {
    int fd;
    {
        SafeFile file(testFile, O_WRONLY | O_CREAT | O_TRUNC);
        fd = file.getFd();
        EXPECT_TRUE(file.isValid());
    }  // Destructor called here
    
    // Try to write to fd (should fail because file is closed)
    char data = 'x';
    ssize_t result = ::write(fd, &data, 1);
    EXPECT_EQ(result, -1);  // Should fail (bad file descriptor)
}

// Test 8: Write to invalid file fails
TEST_F(SafeFileTest, WriteToInvalidFileFails) {
    SafeFile file("/invalid/path.txt", O_WRONLY);
    ASSERT_FALSE(file.isValid());
    
    const char* data = "test";
    ssize_t written = file.write(data, strlen(data));
    
    EXPECT_EQ(written, -1);
}

// Test 9: Read from invalid file fails
TEST_F(SafeFileTest, ReadFromInvalidFileFails) {
    SafeFile file("/invalid/path.txt", O_RDONLY);
    ASSERT_FALSE(file.isValid());
    
    char buffer[10];
    ssize_t bytesRead = file.read(buffer, sizeof(buffer));
    
    EXPECT_EQ(bytesRead, -1);
}