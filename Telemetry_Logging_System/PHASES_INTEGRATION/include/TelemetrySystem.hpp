#ifndef TelemetrySystem_HPP
#define TelemetrySystem_HPP

#include "LogManager.hpp"
#include "ITelemetrySource.hpp"
#include "json.hpp"
#include "SystemTelemetryWriter.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <any>

class TelemetrySystem {

public:
    // Constructor: reads JSON config, builds the entire system
    explicit TelemetrySystem(const std::string& configPath);

    // Starts LogManager + all enabled TelemetryReaders
    void start();

    // Stops all readers first, then stops LogManager cleanly
    void stop();
    void reload(const std::string& configPath); 
    // Destructor: calls stop() if still running
    ~TelemetrySystem();

    // Non-copyable, non-movable (owns threads)
    TelemetrySystem(const TelemetrySystem&)            = delete;
    TelemetrySystem& operator=(const TelemetrySystem&) = delete;
    TelemetrySystem(TelemetrySystem&&)                 = delete;
    TelemetrySystem& operator=(TelemetrySystem&&)      = delete;

private:

    // ── Build helpers ──────────────────────────────────────────────
    // Called from constructor, each reads its section of the JSON

    void buildLogManager(const nlohmann::json& cfg);
    // Reads "log_manager" + "sinks" → builds LogManager via builder

    void buildSources(const nlohmann::json& cfg);
    // Reads "sources" → creates enabled sources + their readers

    void writerLoop();   // continuously updates the cpu/ram output files
    // ── Owned data ─────────────────────────────────────────────────

    std::unique_ptr<LogManager> logManager_;
    // The single LogManager for the whole application

    std::vector<std::unique_ptr<ITelemetrySource>> sources_;
    // Owns all source objects (FileTelemetrySourceImpl, etc.)
    // Base class pointer = polymorphism, any source type fits here

    std::vector<std::shared_ptr<void>> readers_;
    // Owns all TelemetryReader<Policy> objects
    // std::any = type-erased container, holds any type safely
    // We can't use a base class here because TelemetryReader is a template

    std::vector<std::function<void()>> startFns_;
    // One lambda per reader: calls reader->start()
    // We call all of these in start()

    std::vector<std::function<void()>> stopFns_;
    // One lambda per reader: calls reader->stop()
    // We call all of these in stop()

    bool running_ = false;
    // Guards against calling start() twice or stop() before start()

std::unique_ptr<SystemTelemetryWriter> cpuWriter_;
std::unique_ptr<SystemTelemetryWriter> ramWriter_;
std::thread                            writerThread_;
std::atomic<bool>                      writerRunning_{false};

};

#endif