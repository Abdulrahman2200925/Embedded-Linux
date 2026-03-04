#include "TelemetrySystem.hpp"
#include "LogManagerBuilder.hpp"
#include "LogSinkFactory.hpp"
#include "LogSinkType_enum.hpp"

// Telemetry Sources
#include "FileTelemetrySourceImpl.hpp"
#include "SocketTelemetrySourceImpl.hpp"
#include "SomeIPTelemetrySourceImpl.hpp"
#include "SystemTelemetryWriter.hpp"

// Telemetry Readers
#include "TelemetryReader.hpp"

// Policies
#include "CpuPolicy.hpp"
#include "RamPolicy.hpp"
#include "GpuPolicy.hpp"

#include <any>

#include <iostream>
#include <stdexcept>
#include <fstream>
#include "json.hpp" 

// Constructor — the only job is to read the JSON and build everything
TelemetrySystem::TelemetrySystem(const std::string& configPath) {

    // Step 1: open and parse the JSON file
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error("TelemetrySystem: Cannot open config: " + configPath);
    }

    nlohmann::json cfg;
    configFile >> cfg;   // nlohmann parses the whole file into cfg

    // Step 2: build LogManager + sinks from "log_manager" + "sinks" sections
    buildLogManager(cfg);

    // Step 3: build sources + readers from "sources" section
    buildSources(cfg);

    std::cout << "[TelemetrySystem] Initialized from: " << configPath << "\n";
}


void TelemetrySystem::buildLogManager(const nlohmann::json& cfg) {

    // Read log_manager section
    size_t bufferSize    = cfg["log_manager"]["buffer_size"];
    size_t threadPoolSize = cfg["log_manager"]["thread_pool_size"];

    LogManagerBuilder builder;
    builder.setBufferSize(bufferSize)
           .setThreadPoolSize(threadPoolSize);

    // Read sinks section — add each enabled sink
    const auto& sinks = cfg["sinks"];

    if (sinks["console"]["enabled"]) {
        builder.addSink(LogSinkFactory::create(LogSinkType_enum::Console));
    }

    if (sinks["file"]["enabled"]) {
        std::string path = sinks["file"]["path"];
        builder.addSink(LogSinkFactory::create(LogSinkType_enum::File, path));
    }

    if (sinks["socket"]["enabled"]) {
        std::string path = sinks["socket"]["path"];
        builder.addSink(LogSinkFactory::create(LogSinkType_enum::Socket, path));
    }

    // Build and store
    logManager_ = builder.build();

    std::cout << "[TelemetrySystem] LogManager built.\n";
}

void TelemetrySystem::buildSources(const nlohmann::json& cfg) {

    const auto& sources = cfg["sources"];

      // Create the writer that feeds /proc/stat → output files
    cpuWriter_  = std::make_unique<SystemTelemetryWriter>("/tmp/cpu_telemetry.txt");
    ramWriter_  = std::make_unique<SystemTelemetryWriter>("/tmp/ram_telemetry.txt");

    // ── CPU ──────────────────────────────────────────────────────
    if (sources["cpu"]["enabled"]) {

        std::string path     = sources["cpu"]["path"];
        int         rateMs   = sources["cpu"]["parse_rate_ms"];

        // Create the source, store ownership in sources_
        auto cpuSource = std::make_unique<FileTelemetrySourceImpl>(path);
        ITelemetrySource* rawPtr = cpuSource.get();   // keep raw ptr for lambda
        sources_.push_back(std::move(cpuSource));      // sources_ now owns it

        // Create the reader
        auto reader = std::make_unique<TelemetryReader<CpuPolicy>>(
            *rawPtr,
            *logManager_,
            std::chrono::milliseconds(rateMs)
        );

        // Capture raw pointer in lambdas (safe — reader lives in readers_)
        TelemetryReader<CpuPolicy>* readerPtr = reader.get();
        startFns_.push_back([readerPtr]() { readerPtr->start(); });
        stopFns_.push_back([readerPtr]() { readerPtr->stop(); });

        // Store ownership in readers_ via std::any
        readers_.push_back(std::shared_ptr<void>(std::move(reader)));
        

        std::cout << "[TelemetrySystem] CPU source enabled.\n";
    }

    // ── RAM ──────────────────────────────────────────────────────
    if (sources["ram"]["enabled"]) {

        std::string path   = sources["ram"]["path"];
        int         rateMs = sources["ram"]["parse_rate_ms"];

        auto ramSource = std::make_unique<FileTelemetrySourceImpl>(path);
        ITelemetrySource* rawPtr = ramSource.get();
        sources_.push_back(std::move(ramSource));

        auto reader = std::make_unique<TelemetryReader<RamPolicy>>(
            *rawPtr,
            *logManager_,
            std::chrono::milliseconds(rateMs)
        );

        TelemetryReader<RamPolicy>* readerPtr = reader.get();
        startFns_.push_back([readerPtr]() { readerPtr->start(); });
        stopFns_.push_back([readerPtr]() { readerPtr->stop(); });

        readers_.push_back(std::shared_ptr<void>(std::move(reader)));

        std::cout << "[TelemetrySystem] RAM source enabled.\n";
    }

    // ── GPU via SOME/IP ───────────────────────────────────────────
    if (sources["gpu_someip"]["enabled"]) {

        int rateMs = sources["gpu_someip"]["parse_rate_ms"];

        // SomeIPTelemetrySourceImpl is a Singleton — no unique_ptr
        // We don't push to sources_ because we don't own it
        ITelemetrySource& gpuSource = SomeIPTelemetrySourceImpl::getInstance();

        auto reader = std::make_unique<TelemetryReader<GpuPolicy>>(
            gpuSource,
            *logManager_,
            std::chrono::milliseconds(rateMs)
        );

        TelemetryReader<GpuPolicy>* readerPtr = reader.get();
        startFns_.push_back([readerPtr]() { readerPtr->start(); });
        stopFns_.push_back([readerPtr]() { readerPtr->stop(); });

        readers_.push_back(std::shared_ptr<void>(std::move(reader)));

        std::cout << "[TelemetrySystem] GPU SOME/IP source enabled.\n";
    }

    // ── Socket ────────────────────────────────────────────────────
    if (sources["socket"]["enabled"]) {

        std::string path   = sources["socket"]["path"];
        int         rateMs = sources["socket"]["parse_rate_ms"];

        auto socketSource = std::make_unique<SocketTelemetrySourceImpl>(path);
        ITelemetrySource* rawPtr = socketSource.get();
        sources_.push_back(std::move(socketSource));

        auto reader = std::make_unique<TelemetryReader<GpuPolicy>>(
            *rawPtr,
            *logManager_,
            std::chrono::milliseconds(rateMs)
        );

        TelemetryReader<GpuPolicy>* readerPtr = reader.get();
        startFns_.push_back([readerPtr]() { readerPtr->start(); });
        stopFns_.push_back([readerPtr]() { readerPtr->stop(); });

        readers_.push_back(std::shared_ptr<void>(std::move(reader)));

        std::cout << "[TelemetrySystem] Socket source enabled.\n";
    }
}
void TelemetrySystem::writerLoop() {
    while (writerRunning_.load()) {

        // Write CPU data to /tmp/cpu_telemetry.txt
        if (cpuWriter_ && cpuWriter_->isOpen()) {
            cpuWriter_->writeCpuTelemetry();
        }

        // Write RAM data to /tmp/ram_telemetry.txt
        if (ramWriter_ && ramWriter_->isOpen()) {
            ramWriter_->writeMemoryTelemetry();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
void TelemetrySystem::start() {
    if (running_) return;

    // Start the telemetry writer thread first
    writerRunning_ = true;
    writerThread_ = std::thread(&TelemetrySystem::writerLoop, this);

    logManager_->start();
    for (auto& fn : startFns_) fn();

    running_ = true;
    std::cout << "[TelemetrySystem] Started.\n";
}

void TelemetrySystem::stop() {
    if (!running_) return;

    SomeIPTelemetrySourceImpl::getInstance().requestShutdown();

    // Stop writer thread first
    writerRunning_ = false;
    if (writerThread_.joinable()) writerThread_.join();

    for (auto& fn : stopFns_) fn();
    logManager_->stop();

    running_ = false;
    std::cout << "[TelemetrySystem] Stopped.\n";
}
void TelemetrySystem::reload(const std::string& configPath) {

    std::cout << "[TelemetrySystem] Reloading...\n";

    // Step 1: Clear all existing components
    sources_.clear();       // destroy all source objects
    readers_.clear();       // destroy all reader objects
    startFns_.clear();      // clear start lambdas
    stopFns_.clear();       // clear stop lambdas
    cpuWriter_.reset();     // destroy CPU writer
    ramWriter_.reset();     // destroy RAM writer

    // Step 2: Re-parse the JSON
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error(
            "[TelemetrySystem] reload: Cannot open config: " + configPath
        );
    }

    nlohmann::json cfg;
    configFile >> cfg;

    // Step 3: Rebuild LogManager + sinks
    buildLogManager(cfg);

    // Step 4: Rebuild sources + readers
    buildSources(cfg);

    std::cout << "[TelemetrySystem] Reloaded from: " << configPath << "\n";
}
TelemetrySystem::~TelemetrySystem() {
    stop();   // safe — stop() checks running_ flag
}