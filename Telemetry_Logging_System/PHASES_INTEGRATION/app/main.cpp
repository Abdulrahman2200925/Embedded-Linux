#include "LogManager.hpp"
#include "LogSinkFactory.hpp"
#include "TelemetryReader.hpp"
#include "FileTelemetrySourceImpl.hpp"
#include "SocketTelemetrySourceImpl.hpp"
#include "SystemTelemetryWriter.hpp"
#include "CpuPolicy.hpp"
#include "RamPolicy.hpp"
#include "GpuPolicy.hpp"

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>

// Global flag for graceful shutdown
std::atomic<bool> shutdownRequested(false);

// Signal handler for Ctrl+C
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n[MAIN] Shutdown signal received...\n";
        shutdownRequested.store(true);
    }
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "    Phase 4: Asynchronous Logging System\n";
    std::cout << "=================================================\n\n";

    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        // =====================================================
        // STEP 1: Create LogManager
        // =====================================================
        std::cout << "[MAIN] Creating LogManager...\n";
        LogManager logManager(
            100,  // Ring buffer size: 100 messages
            4     // Thread pool size: 4 worker threads
        );

        // =====================================================
        // STEP 2: Add Sinks to LogManager
        // =====================================================
        std::cout << "[MAIN] Adding sinks...\n";
        
        // Console sink
        auto consoleSink = LogSinkFactory::create(LogSinkType_enum::Console);
        logManager.addSink(consoleSink);
        
        // File sink
        auto fileSink = LogSinkFactory::create(
            LogSinkType_enum::File, 
            "system_telemetry.log"
        );
        logManager.addSink(fileSink);
        
        // Socket sink (optional - if you have a listener)
        // auto socketSink = LogSinkFactory::create(
        //     LogSinkType_enum::Socket,
        //     "/tmp/log.sock"
        // );
        // logManager.addSink(socketSink);

        std::cout << "[MAIN] Sinks added successfully\n\n";

        // =====================================================
        // STEP 3: Start LogManager Consumer Thread
        // =====================================================
        std::cout << "[MAIN] Starting LogManager consumer thread...\n";
        logManager.start();
        std::cout << "[MAIN] LogManager started\n\n";

        // =====================================================
        // STEP 4: Create Telemetry Sources
        // =====================================================
        std::cout << "[MAIN] Creating telemetry sources...\n";
        
        // File-based sources (for reading pre-generated data)
        FileTelemetrySourceImpl cpuSource("/tmp/cpu_telemetry.txt");
        FileTelemetrySourceImpl ramSource("/tmp/ram_telemetry.txt");
        FileTelemetrySourceImpl gpuSource("/tmp/gpu_telemetry.txt");
        
        std::cout << "[MAIN] Telemetry sources created\n\n";

        // =====================================================
        // STEP 5: Create SystemTelemetryWriter (Optional)
        // To continuously generate telemetry data
        // =====================================================
        std::cout << "[MAIN] Setting up telemetry data generation...\n";
        
        SystemTelemetryWriter cpuWriter("/tmp/cpu_telemetry.txt");
        SystemTelemetryWriter ramWriter("/tmp/ram_telemetry.txt");
        
        if (!cpuWriter.isOpen() || !ramWriter.isOpen()) {
            std::cerr << "[MAIN] Failed to open telemetry writers\n";
            return 1;
        }
        
        // Start a thread to continuously generate telemetry data
        std::thread telemetryGeneratorThread([&]() {
            std::cout << "[GENERATOR] Telemetry generator thread started\n";
            
            while (!shutdownRequested.load()) {
                // Write CPU telemetry
                cpuWriter.writeCpuTelemetry();
                
                // Write RAM telemetry
                ramWriter.writeMemoryTelemetry();
                
                // Sleep before next write
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            std::cout << "[GENERATOR] Telemetry generator thread stopped\n";
        });
        
        // Give generator time to create initial data
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        std::cout << "[MAIN] Telemetry generation started\n\n";

        // =====================================================
        // STEP 6: Create TelemetryReaders with Different Policies
        // =====================================================
        std::cout << "[MAIN] Creating telemetry readers...\n";
        
        // CPU Reader with CpuPolicy (reads every 1 second)
        TelemetryReader<CpuPolicy> cpuReader(
            cpuSource,
            std::ref(logManager),
            std::chrono::milliseconds(1000)
        );
        
        // RAM Reader with RamPolicy (reads every 1.5 seconds)
        TelemetryReader<RamPolicy> ramReader(
            ramSource,
            std::ref(logManager),
            std::chrono::milliseconds(1500)
        );
        
        // GPU Reader with GpuPolicy (reads every 2 seconds)
        TelemetryReader<GpuPolicy> gpuReader(
            gpuSource,
            std::ref(logManager),
            std::chrono::milliseconds(2000)
        );
        
        std::cout << "[MAIN] Telemetry readers created\n\n";

        // =====================================================
        // STEP 7: Start All Telemetry Readers
        // =====================================================
        std::cout << "[MAIN] Starting telemetry readers...\n";
        
        cpuReader.start();
        std::cout << "[MAIN] CPU reader started\n";
        
        ramReader.start();
        std::cout << "[MAIN] RAM reader started\n";
        
        gpuReader.start();
        std::cout << "[MAIN] GPU reader started\n";
        
        std::cout << "\n[MAIN] All readers started successfully!\n";
        std::cout << "[MAIN] System is now logging telemetry data...\n";
        std::cout << "[MAIN] Press Ctrl+C to stop\n\n";
        
        std::cout << "=================================================\n\n";

        // =====================================================
        // STEP 8: Run Until Shutdown Signal
        // =====================================================
        while (!shutdownRequested.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // =====================================================
        // STEP 9: Graceful Shutdown
        // =====================================================
        std::cout << "\n=================================================\n";
        std::cout << "    Initiating Graceful Shutdown...\n";
        std::cout << "=================================================\n\n";

        // Stop telemetry generator
        std::cout << "[MAIN] Stopping telemetry generator...\n";
        if (telemetryGeneratorThread.joinable()) {
            telemetryGeneratorThread.join();
        }
        std::cout << "[MAIN] Telemetry generator stopped\n\n";

        // Stop readers (in reverse order of starting)
        std::cout << "[MAIN] Stopping telemetry readers...\n";
        
        gpuReader.stop();
        std::cout << "[MAIN] GPU reader stopped\n";
        
        ramReader.stop();
        std::cout << "[MAIN] RAM reader stopped\n";
        
        cpuReader.stop();
        std::cout << "[MAIN] CPU reader stopped\n\n";

        // Stop LogManager (drains buffer and stops consumer)
        std::cout << "[MAIN] Stopping LogManager...\n";
        logManager.stop();
        std::cout << "[MAIN] LogManager stopped\n\n";

        std::cout << "=================================================\n";
        std::cout << "    Shutdown Complete!\n";
        std::cout << "=================================================\n";

    } catch (const std::exception& e) {
        std::cerr << "[MAIN] Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}