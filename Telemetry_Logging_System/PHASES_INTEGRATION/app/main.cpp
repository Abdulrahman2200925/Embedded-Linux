#include "LogManager.hpp"
#include "LogMessage.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SystemTelemetryWriter.hpp"
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
    std::cout << "║  Phase 1 + Phase 2 + Bonus Test       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    // Run all tests
    test_phase1();
    test_phase2();
    test_bonus();
    
    // Summary
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║          All Tests Complete!           ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\n📄 Output Files:" << std::endl;
    std::cout << "  • phase1_test.log          (Phase 1 logs)" << std::endl;
    std::cout << "  • phase2_test.log          (Phase 2 telemetry)" << std::endl;
    std::cout << "  • system_telemetry.log     (Real system data)" << std::endl;
    std::cout << "  • /tmp/system_telemetry.dat (Parsed /proc data)" << std::endl;
    
    std::cout << "\n🔍 Verify real data:" << std::endl;
    std::cout << "  cat /tmp/system_telemetry.dat" << std::endl;
    std::cout << "  cat system_telemetry.log" << std::endl;
    
    return 0;
}