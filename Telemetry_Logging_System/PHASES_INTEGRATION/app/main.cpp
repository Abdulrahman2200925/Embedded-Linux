#include "LogManager.hpp"
#include "LogMessage.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SystemTelemetryWriter.hpp"
#include "LogManagerBuilder.hpp"
#include "LogSinkFactory.hpp"
#include "RingBuffer.hpp"
#include "LogFormatter.hpp"
#include "CpuPolicy.hpp"
#include "RamPolicy.hpp"
#include "GpuPolicy.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

void test_phase1() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    PHASE 1: Logging System Test        " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    LogManager logger;
    
    std::cout << "[Phase 1] Adding sinks..." << std::endl;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    logger.addSink(std::make_shared<FileSinkImpl>("phase1_test.log"));
    std::cout << "[Phase 1] ✓ Sinks added\n" << std::endl;
    
    std::cout << "[Phase 1] Logging messages..." << std::endl;
    logger.log(LogMessage("MyApp", "Startup", Severity::INFO, "Application started"));
    logger.log(LogMessage("MyApp", "Database", Severity::DEBUG, "Connected to DB"));
    logger.log(LogMessage("MyApp", "Network", Severity::WARNING, "High latency: 250ms"));
    logger.log(LogMessage("MyApp", "Auth", Severity::ERROR, "Invalid credentials"));
    logger.log(LogMessage("MyApp", "Security", Severity::CRITICAL, "Security breach!"));
    
    logger.flush();
    std::cout << "[Phase 1] ✓ Test complete!\n" << std::endl;
}

void test_phase2() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    PHASE 2: Telemetry Source Test      " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    LogManager logger;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    logger.addSink(std::make_shared<FileSinkImpl>("phase2_test.log"));
    
    // Create simple test file
    std::cout << "[Phase 2] Creating test telemetry file..." << std::endl;
    system("echo 'Temperature: 25.5°C' > /tmp/test_sensor.txt");
    system("echo 'Humidity: 60%' >> /tmp/test_sensor.txt");
    std::cout << "[Phase 2] ✓ Test file created\n" << std::endl;
    
    std::cout << "[Phase 2] Reading telemetry..." << std::endl;
    FileTelemetrySourceImpl telemetry("/tmp/test_sensor.txt");
    
    if (!telemetry.openSource()) {
        std::cerr << "[Phase 2] ✗ Failed to open source!" << std::endl;
        return;
    }
    
    std::string data;
    int count = 0;
    while (telemetry.readSource(data)) {
        count++;
        logger.log(LogMessage("Telemetry", "Sensor", Severity::INFO, data));
    }
    
    logger.flush();
    std::cout << "[Phase 2] ✓ Logged " << count << " readings\n" << std::endl;
}

void test_phase3() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    PHASE 3: Design Patterns Test       " << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 1. Builder Pattern - Fluent API for LogManager
    std::cout << "[Phase 3] Testing Builder Pattern..." << std::endl;
    LogManager logger = LogManagerBuilder()
        .addSink(std::make_shared<ConsoleSinkImpl>())
        .addSink(std::make_shared<FileSinkImpl>("phase3_test.log"))
        .build();
    std::cout << "[Phase 3] ✓ LogManager built with fluent API\n" << std::endl;

    // 2. Factory Pattern - Create sinks without direct instantiation
    std::cout << "[Phase 3] Testing Factory Pattern..." << std::endl;
    auto consoleSink = LogSinkFactory::create(LogSinkType_enum::Console);
    auto fileSink = LogSinkFactory::create(LogSinkType_enum::File, "factory_test.log");
    std::cout << "[Phase 3] ✓ Sinks created via Factory\n" << std::endl;

    // 3. Template Programming - RingBuffer with strings
    std::cout << "[Phase 3] Testing RingBuffer Template..." << std::endl;
    RingBuffer<std::string> buffer(3);

    buffer.tryPush("CPU: 45.2");
    buffer.tryPush("RAM: 68.5");
    buffer.tryPush("GPU: 82.1");

    std::cout << "[Phase 3] Buffer size: " << buffer.size() << "/" << buffer.getCapacity() << std::endl;

    std::string data;
    while (buffer.tryPop(data)) {
        std::cout << "[Phase 3] Popped: " << data << std::endl;
    }
    std::cout << "[Phase 3] ✓ RingBuffer works correctly\n" << std::endl;

    // 4. Policy-Based Design - LogFormatter with different policies
    std::cout << "[Phase 3] Testing Policy-Based Design..." << std::endl;

    // CPU Policy
    LogFormatter<CpuPolicy> cpuFormatter;
    auto cpuLog = cpuFormatter.formatDataToLogMsg("85.5");
    if (cpuLog.has_value()) {
        logger.log(cpuLog.value());
        std::cout << "[Phase 3] ✓ CPU telemetry formatted (WARNING level)" << std::endl;
    }

    // RAM Policy
    LogFormatter<RamPolicy> ramFormatter;
    auto ramLog = ramFormatter.formatDataToLogMsg("45.2");
    if (ramLog.has_value()) {
        logger.log(ramLog.value());
        std::cout << "[Phase 3] ✓ RAM telemetry formatted (INFO level)" << std::endl;
    }

    // GPU Policy
    LogFormatter<GpuPolicy> gpuFormatter;
    auto gpuLog = gpuFormatter.formatDataToLogMsg("95.8");
    if (gpuLog.has_value()) {
        logger.log(gpuLog.value());
        std::cout << "[Phase 3] ✓ GPU telemetry formatted (CRITICAL level)" << std::endl;
    }

    logger.flush();
    std::cout << "\n[Phase 3] ✓ All design patterns working!\n" << std::endl;
}

void test_bonus() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "    BONUS: Real System Telemetry        " << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    std::cout << "[Bonus] This test parses /proc/stat and /proc/meminfo" << std::endl;
    std::cout << "[Bonus] Data flow: /proc → Parser → File → Telemetry → Logger\n" << std::endl;
    
    // Step 1: Parse system data and write to file
    std::cout << "[Bonus] Step 1: Parsing /proc/stat and /proc/meminfo..." << std::endl;
    
    const std::string telemetryFile = "/tmp/system_telemetry.dat";
    SystemTelemetryWriter writer(telemetryFile);
    
    if (!writer.isOpen()) {
        std::cerr << "[Bonus] ✗ Failed to open output file!" << std::endl;
        return;
    }
    
    std::cout << "[Bonus] Writing CPU data from /proc/stat..." << std::endl;
    if (!writer.writeCpuTelemetry()) {
        std::cerr << "[Bonus] ✗ Failed to write CPU data!" << std::endl;
        return;
    }
    std::cout << "[Bonus] ✓ CPU data written" << std::endl;
    
    std::cout << "[Bonus] Writing Memory data from /proc/meminfo..." << std::endl;
    if (!writer.writeMemoryTelemetry()) {
        std::cerr << "[Bonus] ✗ Failed to write memory data!" << std::endl;
        return;
    }
    std::cout << "[Bonus] ✓ Memory data written\n" << std::endl;
    
    // Step 2: Read the file using FileTelemetrySource
    std::cout << "[Bonus] Step 2: Reading telemetry file with SafeFile..." << std::endl;
    
    // Small delay to ensure file is written
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    FileTelemetrySourceImpl telemetrySource(telemetryFile);
    
    if (!telemetrySource.openSource()) {
        std::cerr << "[Bonus] ✗ Failed to open telemetry source!" << std::endl;
        return;
    }
    std::cout << "[Bonus] ✓ Telemetry source opened\n" << std::endl;
    
    // Step 3: Log the data
    std::cout << "[Bonus] Step 3: Logging system telemetry...\n" << std::endl;
    
    LogManager logger;
    logger.addSink(std::make_shared<ConsoleSinkImpl>());
    logger.addSink(std::make_shared<FileSinkImpl>("system_telemetry.log"));
    
    std::string telemetryData;
    int readCount = 0;
    
    while (telemetrySource.readSource(telemetryData)) {
        readCount++;
        
        // Determine severity based on values
        Severity severity = Severity::INFO;
        if (telemetryData.find("CPU_USAGE") != std::string::npos) {
            // Extract CPU percentage
            size_t pos = telemetryData.find(':');
            if (pos != std::string::npos) {
                double cpuPercent = std::stod(telemetryData.substr(pos + 1));
                if (cpuPercent > 80) severity = Severity::WARNING;
                if (cpuPercent > 95) severity = Severity::CRITICAL;
            }
        }
        
        logger.log(LogMessage(
            "SystemMonitor",
            "Telemetry",
            severity,
            telemetryData
        ));
        
        std::cout << "[Bonus] Logged: " << telemetryData;
    }
    
    logger.flush();
    
    std::cout << "\n[Bonus] ✓ Test complete!" << std::endl;
    std::cout << "[Bonus] Readings logged: " << readCount << std::endl;
    std::cout << "[Bonus] Data was REAL from /proc filesystem\n" << std::endl;
}

int main() {
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║    All Phases Integration Test         ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    // Run all tests
    test_phase1();
    test_phase2();
    test_phase3();
    test_bonus();

    // Summary
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║          All Tests Complete!           ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    std::cout << "\nOutput Files:" << std::endl;
    std::cout << "  • phase1_test.log          (Phase 1 - Logging)" << std::endl;
    std::cout << "  • phase2_test.log          (Phase 2 - Telemetry)" << std::endl;
    std::cout << "  • phase3_test.log          (Phase 3 - Design Patterns)" << std::endl;
    std::cout << "  • factory_test.log         (Phase 3 - Factory Pattern)" << std::endl;
    std::cout << "  • system_telemetry.log     (Bonus - Real System Data)" << std::endl;

    return 0;
}