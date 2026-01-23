#include <gtest/gtest.h>
#include "LogSinkType_enum.hpp"
#include "SeverityLvl_enum.hpp"
#include "TelemetrySrc_enum.hpp"
#include "CpuPolicy.hpp"
#include "GpuPolicy.hpp"
#include "RamPolicy.hpp"
#include "LogFormatter.hpp"
#include "LogSinkFactory.hpp"
#include "LogManagerBuilder.hpp"
#include "RingBuffer.hpp"

// ============================================================================
// TEST SUITE 1: ENUMS
// ============================================================================

TEST(Phase3_Enums, LogSinkType_Values) {
    LogSinkType_enum console = LogSinkType_enum::Console;
    LogSinkType_enum file = LogSinkType_enum::File;
    LogSinkType_enum socket = LogSinkType_enum::Socket;
    
    EXPECT_EQ(console, LogSinkType_enum::Console);
    EXPECT_EQ(file, LogSinkType_enum::File);
    EXPECT_EQ(socket, LogSinkType_enum::Socket);
    EXPECT_NE(console, file);
}

TEST(Phase3_Enums, SeverityLvl_Values) {
    SeverityLvl_enum info = SeverityLvl_enum::INFO;
    SeverityLvl_enum warning = SeverityLvl_enum::WARNING;
    SeverityLvl_enum critical = SeverityLvl_enum::CRITICAL;
    
    EXPECT_EQ(info, SeverityLvl_enum::INFO);
    EXPECT_EQ(warning, SeverityLvl_enum::WARNING);
    EXPECT_EQ(critical, SeverityLvl_enum::CRITICAL);
    EXPECT_NE(info, critical);
}

TEST(Phase3_Enums, TelemetrySrc_Values) {
    TelemetrySrc_enum cpu = TelemetrySrc_enum::CPU;
    TelemetrySrc_enum gpu = TelemetrySrc_enum::GPU;
    TelemetrySrc_enum ram = TelemetrySrc_enum::RAM;
    
    EXPECT_EQ(cpu, TelemetrySrc_enum::CPU);
    EXPECT_EQ(gpu, TelemetrySrc_enum::GPU);
    EXPECT_EQ(ram, TelemetrySrc_enum::RAM);
    EXPECT_NE(cpu, gpu);
}

// ============================================================================
// TEST SUITE 2: POLICIES
// ============================================================================

TEST(Phase3_Policies, CpuPolicy_StaticValues) {
    EXPECT_EQ(CpuPolicy::context, TelemetrySrc_enum::CPU);
    EXPECT_EQ(CpuPolicy::unit, "%");
    EXPECT_FLOAT_EQ(CpuPolicy::WARNING, 75.0f);
    EXPECT_FLOAT_EQ(CpuPolicy::CRITICAL, 90.0f);
}

TEST(Phase3_Policies, CpuPolicy_InferSeverity) {
    EXPECT_EQ(CpuPolicy::inferSeverity(50.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(CpuPolicy::inferSeverity(75.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(CpuPolicy::inferSeverity(75.1f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(80.0f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(90.0f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(CpuPolicy::inferSeverity(90.1f), SeverityLvl_enum::CRITICAL);
    EXPECT_EQ(CpuPolicy::inferSeverity(95.0f), SeverityLvl_enum::CRITICAL);
}

TEST(Phase3_Policies, GpuPolicy_StaticValues) {
    EXPECT_EQ(GpuPolicy::context, TelemetrySrc_enum::GPU);
    EXPECT_EQ(GpuPolicy::unit, "%");
    EXPECT_FLOAT_EQ(GpuPolicy::WARNING, 80.0f);
    EXPECT_FLOAT_EQ(GpuPolicy::CRITICAL, 95.0f);
}

TEST(Phase3_Policies, GpuPolicy_InferSeverity) {
    EXPECT_EQ(GpuPolicy::inferSeverity(70.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(GpuPolicy::inferSeverity(85.0f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(GpuPolicy::inferSeverity(98.0f), SeverityLvl_enum::CRITICAL);
}

TEST(Phase3_Policies, RamPolicy_StaticValues) {
    EXPECT_EQ(RamPolicy::context, TelemetrySrc_enum::RAM);
    EXPECT_EQ(RamPolicy::unit, "%");
    EXPECT_FLOAT_EQ(RamPolicy::WARNING, 70.0f);
    EXPECT_FLOAT_EQ(RamPolicy::CRITICAL, 85.0f);
}

TEST(Phase3_Policies, RamPolicy_InferSeverity) {
    EXPECT_EQ(RamPolicy::inferSeverity(60.0f), SeverityLvl_enum::INFO);
    EXPECT_EQ(RamPolicy::inferSeverity(75.0f), SeverityLvl_enum::WARNING);
    EXPECT_EQ(RamPolicy::inferSeverity(90.0f), SeverityLvl_enum::CRITICAL);
}

// ============================================================================
// TEST SUITE 3: LOG FORMATTER
// ============================================================================

TEST(Phase3_LogFormatter, CpuFormatter_ValidInput_InfoLevel) {
    LogFormatter<CpuPolicy> formatter;
    
    auto msg = formatter.formatDataToLogMsg("50.5");
    
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->getAppName(), "CPU");
    EXPECT_EQ(msg->getContext(), "Telemetry");
    EXPECT_EQ(msg->getSeverity(), Severity::INFO);
    EXPECT_NE(msg->getText().find("50.50"), std::string::npos);
}

TEST(Phase3_LogFormatter, CpuFormatter_ValidInput_WarningLevel) {
    LogFormatter<CpuPolicy> formatter;
    
    auto msg = formatter.formatDataToLogMsg("80.0");
    
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->getSeverity(), Severity::WARNING);
}

TEST(Phase3_LogFormatter, CpuFormatter_ValidInput_CriticalLevel) {
    LogFormatter<CpuPolicy> formatter;
    
    auto msg = formatter.formatDataToLogMsg("95.5");
    
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->getSeverity(), Severity::CRITICAL);
}

TEST(Phase3_LogFormatter, CpuFormatter_InvalidInput) {
    LogFormatter<CpuPolicy> formatter;
    
    auto msg1 = formatter.formatDataToLogMsg("invalid");
    EXPECT_FALSE(msg1.has_value());
    
    auto msg2 = formatter.formatDataToLogMsg("");
    EXPECT_FALSE(msg2.has_value());
    
    auto msg3 = formatter.formatDataToLogMsg("abc123");
    EXPECT_FALSE(msg3.has_value());
}

TEST(Phase3_LogFormatter, GpuFormatter_CorrectContext) {
    LogFormatter<GpuPolicy> formatter;
    
    auto msg = formatter.formatDataToLogMsg("70.0");
    
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->getAppName(), "GPU");
    EXPECT_EQ(msg->getSeverity(), Severity::INFO);
}

TEST(Phase3_LogFormatter, RamFormatter_CorrectContext) {
    LogFormatter<RamPolicy> formatter;
    
    auto msg = formatter.formatDataToLogMsg("65.0");
    
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->getAppName(), "RAM");
    EXPECT_EQ(msg->getSeverity(), Severity::INFO);
}

// ============================================================================
// TEST SUITE 4: FACTORY PATTERN
// ============================================================================

TEST(Phase3_Factory, CreateConsoleSink) {
    auto sink = LogSinkFactory::create(LogSinkType_enum::Console);
    
    ASSERT_NE(sink, nullptr);
    
    // Test it can write
    LogMessage testMsg("Test", "Factory", Severity::INFO, "Console test");
    EXPECT_NO_THROW(sink->write(testMsg));
}

TEST(Phase3_Factory, CreateFileSink) {
    auto sink = LogSinkFactory::create(LogSinkType_enum::File, "test_factory.log");
    
    ASSERT_NE(sink, nullptr);
    
    // Test it can write
    LogMessage testMsg("Test", "Factory", Severity::INFO, "File test");
    EXPECT_NO_THROW(sink->write(testMsg));
}

TEST(Phase3_Factory, FileSink_MissingParameter_ThrowsException) {
    EXPECT_THROW({
        auto sink = LogSinkFactory::create(LogSinkType_enum::File);
    }, std::invalid_argument);
}

TEST(Phase3_Factory, CreateMultipleSinks) {
    auto console = LogSinkFactory::create(LogSinkType_enum::Console);
    auto file1 = LogSinkFactory::create(LogSinkType_enum::File, "test1.log");
    auto file2 = LogSinkFactory::create(LogSinkType_enum::File, "test2.log");
    
    EXPECT_NE(console, nullptr);
    EXPECT_NE(file1, nullptr);
    EXPECT_NE(file2, nullptr);
    EXPECT_NE(console, file1);
}

// ============================================================================
// TEST SUITE 5: BUILDER PATTERN
// ============================================================================

TEST(Phase3_Builder, BasicBuild) {
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
        .build();
    
    EXPECT_NO_THROW({
        logger.log(LogMessage("Test", "Builder", Severity::INFO, "Basic build"));
        logger.flush();
    });
}

TEST(Phase3_Builder, BuildWithMultipleSinks) {
    LogManager logger = LogManagerBuilder()
        .addSink(LogSinkFactory::create(LogSinkType_enum::Console))
        .addSink(LogSinkFactory::create(LogSinkType_enum::File, "test_builder.log"))
        .build();
    
    EXPECT_NO_THROW({
        logger.log(LogMessage("Test", "Builder", Severity::WARNING, "Multiple sinks"));
        logger.flush();
    });
}

TEST(Phase3_Builder, MethodChaining) {
    // Test that method chaining works
    LogManagerBuilder builder;
    
    auto& ref1 = builder.addSink(LogSinkFactory::create(LogSinkType_enum::Console));
    auto& ref2 = ref1.addSink(LogSinkFactory::create(LogSinkType_enum::File, "chain.log"));
    
    // All references should point to same builder
    EXPECT_EQ(&builder, &ref1);
    EXPECT_EQ(&builder, &ref2);
}

TEST(Phase3_Builder, ReusableBuilder) {
    LogManagerBuilder template_builder;
    template_builder.addSink(LogSinkFactory::create(LogSinkType_enum::Console));
    
    // Build multiple loggers from same builder
    LogManager logger1 = template_builder.build();
    LogManager logger2 = template_builder.build();
    
    EXPECT_NO_THROW({
        logger1.log(LogMessage("Logger1", "Test", Severity::INFO, "First"));
        logger2.log(LogMessage("Logger2", "Test", Severity::INFO, "Second"));
        logger1.flush();
        logger2.flush();
    });
}

// ============================================================================
// TEST SUITE 6: RING BUFFER (BONUS)
// ============================================================================

TEST(Phase3_RingBuffer, InitialState) {
    RingBuffer<int> buf(5);
    
    EXPECT_TRUE(buf.isEmpty());
    EXPECT_FALSE(buf.isFull());
    EXPECT_EQ(buf.size(), 0);
    EXPECT_EQ(buf.getCapacity(), 5);
}

TEST(Phase3_RingBuffer, PushPop_BasicOperations) {
    RingBuffer<int> buf(3);
    
    // Push 3 items
    EXPECT_TRUE(buf.tryPush(10));
    EXPECT_TRUE(buf.tryPush(20));
    EXPECT_TRUE(buf.tryPush(30));
    
    EXPECT_TRUE(buf.isFull());
    EXPECT_EQ(buf.size(), 3);
    
    // Try push when full
    EXPECT_FALSE(buf.tryPush(40));
    
    // Pop all items
    int val;
    EXPECT_TRUE(buf.tryPop(val));
    EXPECT_EQ(val, 10);
    
    EXPECT_TRUE(buf.tryPop(val));
    EXPECT_EQ(val, 20);
    
    EXPECT_TRUE(buf.tryPop(val));
    EXPECT_EQ(val, 30);
    
    EXPECT_TRUE(buf.isEmpty());
    
    // Try pop when empty
    EXPECT_FALSE(buf.tryPop(val));
}

TEST(Phase3_RingBuffer, Wraparound) {
    RingBuffer<int> buf(3);
    
    // Fill buffer
    buf.tryPush(1);
    buf.tryPush(2);
    buf.tryPush(3);
    
    // Pop one
    int val;
    buf.tryPop(val);
    EXPECT_EQ(val, 1);
    
    // Push two more (wraps around)
    EXPECT_TRUE(buf.tryPush(4));
    EXPECT_TRUE(buf.tryPush(5));
    
    EXPECT_TRUE(buf.isFull());
    
    // Pop all
    buf.tryPop(val);
    EXPECT_EQ(val, 2);
    buf.tryPop(val);
    EXPECT_EQ(val, 3);
    buf.tryPop(val);
    EXPECT_EQ(val, 4);
    
    EXPECT_FALSE(buf.isFull());
}

TEST(Phase3_RingBuffer, WithLogMessage) {
    RingBuffer<LogMessage> buf(2);
    
    EXPECT_TRUE(buf.tryPush(LogMessage("App1", "Ctx1", Severity::INFO, "Msg1")));
    EXPECT_TRUE(buf.tryPush(LogMessage("App2", "Ctx2", Severity::WARNING, "Msg2")));
    EXPECT_TRUE(buf.isFull());
    
    LogMessage msg("", "", Severity::DEBUG, "");
    
    EXPECT_TRUE(buf.tryPop(msg));
    EXPECT_EQ(msg.getAppName(), "App1");
    EXPECT_EQ(msg.getSeverity(), Severity::INFO);
    
    EXPECT_TRUE(buf.tryPop(msg));
    EXPECT_EQ(msg.getAppName(), "App2");
    EXPECT_EQ(msg.getSeverity(), Severity::WARNING);
    
    EXPECT_TRUE(buf.isEmpty());
}

TEST(Phase3_RingBuffer, MoveSemantics) {
    RingBuffer<int> buf1(3);
    buf1.tryPush(10);
    buf1.tryPush(20);
    
    // Move construction
    RingBuffer<int> buf2(std::move(buf1));
    EXPECT_EQ(buf2.size(), 2);
    
    int val;
    buf2.tryPop(val);
    EXPECT_EQ(val, 10);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}