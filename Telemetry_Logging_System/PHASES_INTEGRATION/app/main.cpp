#include "TelemetrySystem.hpp"
#include <iostream>
#include <fstream>       // std::ofstream
#include <csignal>
#include <thread>
#include <chrono>
#include <atomic>
#include <unistd.h>      // getpid()

// ── Global state ──────────────────────────────────────────────
std::atomic<bool> g_running(true);
std::atomic<bool> g_reload(false);
TelemetrySystem*  g_app = nullptr;

// ── Signal handlers ───────────────────────────────────────────

// Ctrl+C — shutdown
void signalHandler(int signal) {
    std::cout << "\n[Main] Caught signal " << signal << " — shutting down...\n";
    g_running = false;
    if (g_app) {
        g_app->stop();
    }
}

// SIGHUP — reload config (triggered by GUI)
void reloadHandler(int signal) {
    std::cout << "\n[Main] Reload signal received — reloading config...\n";
    g_reload = true;
}

// ── Main ──────────────────────────────────────────────────────
int main(int argc, char* argv[]) {

    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGHUP, reloadHandler);

    // Config path — from argument or default
    std::string configPath = (argc > 1)
        ? argv[1]
        : "/home/mac/workspace/Eng.Fady/CPP_PROJECT/PHASES_INTEGRATION/config/app_config.json";

    // Build the system from JSON
    TelemetrySystem app(configPath);
    g_app = &app;

    // Start everything
    app.start();

    // Save PID so GUI can send SIGHUP
    std::ofstream pidFile("app.pid");
    pidFile << getpid();
    pidFile.close();
    std::cout << "[Main] PID " << getpid() << " saved to app.pid\n";

    std::cout << "[Main] System running. Press Ctrl+C to stop.\n";

    // ── Main loop ─────────────────────────────────────────────
    while (g_running) {

        // GUI requested a config reload
        if (g_reload.load()) {
            g_reload = false;

            std::cout << "[Main] Reloading configuration...\n";
            app.stop();
            app.reload(configPath);
            app.start();
            std::cout << "[Main] Reload complete.\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Cleanup PID file on exit
    std::remove("app.pid");

    std::cout << "[Main] Goodbye.\n";
    return 0;
}