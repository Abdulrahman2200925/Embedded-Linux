#include <gtest/gtest.h>
#include "LogFormatter.hpp"
#include "CpuPolicy.hpp"
#include "GpuPolicy.hpp"
#include "RamPolicy.hpp"
#include "LogSinkFactory.hpp"
#include "LogManagerBuilder.hpp"
#include "LogManager.hpp"
#include "LogMessage.hpp"
#include <fstream>
#include <string>

// ============================================================================
// INTEGRATION TEST 1: Formatter → LogManager
// ============================================================================

TEST(Phase3_Integration, Formatter_To_LogManager) {
    // Setup: Create formatter and logger
    LogFormatter<CpuPolicy> cpuFormatter;
    
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "integration_formatter.log"))
        .build();
    
    // Test: Format telemetry data and log it
    auto msg1 = cpuFormatter.formatDataToLogMsg("55.5");
    ASSERT_TRUE(msg1.has_value());
    logger.log(msg1.value());
    
    auto msg2 = cpuFormatter.formatDataToLogMsg("80.0");
    ASSERT_TRUE(msg2.has_value());
    logger.log(msg2.value());
    
    auto msg3 = cpuFormatter.formatDataToLogMsg("95.0");
    ASSERT_TRUE(msg3.has_value());
    logger.log(msg3.value());
    
    EXPECT_NO_THROW(logger.flush());
    
    // Verify: Check file was created and has content
    std::ifstream file("integration_formatter.log");
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
        EXPECT_FALSE(line.empty());
    }
    
    EXPECT_EQ(lineCount, 3);
    file.close();
}

// ============================================================================
// INTEGRATION TEST 2: Multiple Policies → Logger
// ============================================================================

TEST(Phase3_Integration, MultipleFormatters_To_Logger) {
    // Setup: Create formatters for all policies
    LogFormatter<CpuPolicy> cpuFormatter;
    LogFormatter<GpuPolicy> gpuFormatter;
    LogFormatter<RamPolicy> ramFormatter;
    
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "integration_multi.log"))
        .build();
    
    // Test: Log from all formatters
    auto cpuMsg = cpuFormatter.formatDataToLogMsg("85.0");
    ASSERT_TRUE(cpuMsg.has_value());
    logger.log(cpuMsg.value());
    
    auto gpuMsg = gpuFormatter.formatDataToLogMsg("90.0");
    ASSERT_TRUE(gpuMsg.has_value());
    logger.log(gpuMsg.value());
    
    auto ramMsg = ramFormatter.formatDataToLogMsg("75.0");
    ASSERT_TRUE(ramMsg.has_value());
    logger.log(ramMsg.value());
    
    EXPECT_NO_THROW(logger.flush());
    
    // Verify: All messages have different app names
    EXPECT_EQ(cpuMsg->getAppName(), "CPU");
    EXPECT_EQ(gpuMsg->getAppName(), "GPU");
    EXPECT_EQ(ramMsg->getAppName(), "RAM");
    
    // Verify: All have correct severity based on their policies
    EXPECT_EQ(cpuMsg->getSeverity(), Severity::WARNING);  // 85 > 75 (CPU)
    EXPECT_EQ(gpuMsg->getSeverity(), Severity::WARNING);  // 90 > 80 (GPU)
    EXPECT_EQ(ramMsg->getSeverity(), Severity::WARNING);  // 75 > 70 (RAM)
}

// ============================================================================
// INTEGRATION TEST 3: Factory + Builder + Formatter
// ============================================================================

TEST(Phase3_Integration, Factory_Builder_Formatter_FullPipeline) {
    // Setup: Use Factory to create sinks, Builder to create logger
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "integration_full.log"))
        .build();
    
    // Setup: Use Formatter to create messages
    LogFormatter<CpuPolicy> formatter;
    
    // Test: Simulate telemetry stream
    std::vector<std::string> telemetryData = {
        "45.0",   // INFO
        "78.5",   // WARNING
        "92.0",   // CRITICAL
        "invalid", // Should be skipped
        "60.0",   // INFO
        "95.5"    // CRITICAL
    };
    
    int validMessages = 0;
    for (const auto& data : telemetryData) {
        auto msg = formatter.formatDataToLogMsg(data);
        if (msg.has_value()) {
            logger.log(msg.value());
            validMessages++;
        }
    }
    
    EXPECT_EQ(validMessages, 5);  // 5 valid, 1 invalid
    
    EXPECT_NO_THROW(logger.flush());
    
    // Verify: File contains correct number of messages
    std::ifstream file("integration_full.log");
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    
    EXPECT_EQ(lineCount, 5);
    file.close();
}

// ============================================================================
// INTEGRATION TEST 4: All Phases Together (Phase 1 + 2 + 3)
// ============================================================================

TEST(Phase3_Integration, AllPhases_CompleteSystem) {
    // Phase 3: Create formatters
    LogFormatter<CpuPolicy> cpuFormatter;
    LogFormatter<GpuPolicy> gpuFormatter;
    LogFormatter<RamPolicy> ramFormatter;
    
    // Phase 3: Use Factory and Builder
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "complete_system.log"))
        .build();
    
    // Simulate: Telemetry system sending data
    struct TelemetryReading {
        std::string source;
        std::string value;
    };
    
    std::vector<TelemetryReading> readings = {
        {"CPU", "55.0"},
        {"GPU", "88.0"},
        {"RAM", "72.0"},
        {"CPU", "91.5"},
        {"GPU", "96.0"},
        {"RAM", "88.0"}
    };
    
    // Process each reading
    for (const auto& reading : readings) {
        std::optional<LogMessage> msg;
        
        if (reading.source == "CPU") {
            msg = cpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.source == "GPU") {
            msg = gpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.source == "RAM") {
            msg = ramFormatter.formatDataToLogMsg(reading.value);
        }
        
        if (msg.has_value()) {
            logger.log(msg.value());
        }
    }
    
    EXPECT_NO_THROW(logger.flush());
    
    // Verify: System processed all readings
    std::ifstream file("complete_system.log");
    ASSERT_TRUE(file.is_open());
    
    int lineCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        lineCount++;
        
        // Verify format: Should contain app name, timestamp, context, severity, text
        EXPECT_NE(line.find("["), std::string::npos);
        EXPECT_NE(line.find("Telemetry"), std::string::npos);
    }
    
    EXPECT_EQ(lineCount, 6);
    file.close();
}

// ============================================================================
// INTEGRATION TEST 5: Error Handling Across Components
// ============================================================================

TEST(Phase3_Integration, ErrorHandling_InvalidData) {
    LogFormatter<CpuPolicy> formatter;
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "error_handling.log"))
        .build();
    
    // Test: Mixed valid and invalid data
    std::vector<std::string> mixedData = {
        "50.0",      // Valid
        "invalid",   // Invalid
        "",          // Invalid
        "abc",       // Invalid
        "75.5",      // Valid
        "NaN",       // Invalid
        "85.0"       // Valid
    };
    
    int logged = 0;
    for (const auto& data : mixedData) {
        auto msg = formatter.formatDataToLogMsg(data);
        if (msg.has_value()) {
            logger.log(msg.value());
            logged++;
        }
    }
    
    EXPECT_EQ(logged, 3);  // Only 3 valid messages
    
    logger.flush();
    
    // Verify: Only valid messages in file
    std::ifstream file("error_handling.log");
    int lines = 0;
    std::string line;
    while (std::getline(file, line)) {
        lines++;
    }
    EXPECT_EQ(lines, 3);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}