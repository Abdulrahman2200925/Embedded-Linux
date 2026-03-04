#include "SomeIPTelemetrySourceImpl.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

SomeIPTelemetrySourceImpl& SomeIPTelemetrySourceImpl::getInstance() {
    static SomeIPTelemetrySourceImpl instance;
    return instance;
}

bool SomeIPTelemetrySourceImpl::openSource() {
    std::cout << "[Adapter] openSource() → building CommonAPI proxy...\n";

    auto runtime = CommonAPI::Runtime::get();
    proxy_ = runtime->buildProxy<v1::omnimetron::gpu::GpuUsageDataProxy>(
        "local", "omnimetron.gpu.GpuUsageData"
    );

    if (!proxy_) {
        std::cerr << "[Adapter] ERROR: Failed to build proxy!\n";
        return false;
    }

    std::cout << "[Adapter] Waiting for service...\n";
    for (int i = 0; i < 30; i++) {

        // ← check shutdown flag every iteration
        if (shutdownRequested_.load()) {
            std::cout << "[Adapter] Shutdown requested — aborting wait.\n";
            return false;
        }

        if (proxy_->isAvailable()) {
            std::cout << "[Adapter] Service available!\n";
            break;
        }

        std::cout << "[Adapter] Waiting... (" << (i+1) << "/30)\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (!proxy_->isAvailable()) {
        std::cerr << "[Adapter] Service not available after 30 seconds\n";
        return false;
    }

    latestEventValue_ = 0.0f;
    eventReceived_    = false;
    return true;
}
bool SomeIPTelemetrySourceImpl::readSource(std::string& out) {

    // Check proxy exists and service is still available
    if (!proxy_ || !proxy_->isAvailable()) {
        std::cerr << "[Adapter] Proxy not ready\n";
        return false;
    }

    // Check shutdown wasn't requested
    if (shutdownRequested_.load()) {
        return false;
    }

    CommonAPI::CallStatus callStatus;
    float usage = 0.0f;

    // Make the synchronous RPC call to the service
    proxy_->requestGpuUsageData(callStatus, usage);

    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "[Adapter] requestGpuUsageData failed\n";
        return false;
    }

    // Convert float to string — feeds into LogFormatter<GpuPolicy>
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << usage;
    out = oss.str();

    std::cout << "[Adapter] Got value: " << out << "%\n";
    return true;
}
void SomeIPTelemetrySourceImpl::subscribeToEvents() {

    if (!proxy_) {
        std::cerr << "[Adapter] Cannot subscribe — proxy not built\n";
        return;
    }

    std::cout << "[Adapter] Subscribing to GPU usage broadcast events...\n";

    proxy_->getNotifyGpuUsageDataChangeEvent().subscribe(
        [this](float usage) {
            std::cout << "[Adapter] Event received: GPU usage = "
                      << usage << "%\n";
            std::lock_guard<std::mutex> lock(eventMutex_);
            latestEventValue_ = usage;
            eventReceived_    = true;
        }
    );

    std::cout << "[Adapter] Subscribed successfully\n";
}