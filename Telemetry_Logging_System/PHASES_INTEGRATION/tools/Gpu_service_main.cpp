#include <CommonAPI/CommonAPI.hpp>
#include "GpuServiceImpl.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {

    std::cout << "========================================\n";
    std::cout << "  GPU Telemetry Service (CommonAPI)\n";
    std::cout << "========================================\n\n";

    // -------------------------------------------------------------------------
    // Config is set externally via environment variables — do NOT hardcode paths
    // This allows the binary to work on both PC and RPi3 without recompilation:
    //
    // On PC:
    //   VSOMEIP_CONFIGURATION=../config/vsomeip-service.json
    //   COMMONAPI_CONFIG=../config/commonapi-someip-service.ini
    //
    // On RPi3:
    //   VSOMEIP_CONFIGURATION=/home/mac/vsomeip-service.json
    //   COMMONAPI_CONFIG=/home/mac/commonapi-someip-service.ini
    // -------------------------------------------------------------------------

    auto runtime = CommonAPI::Runtime::get();

    std::shared_ptr<GpuServiceImpl> service = std::make_shared<GpuServiceImpl>();

    const std::string domain   = "local";
    const std::string instance = "omnimetron.gpu.GpuUsageData";

    std::cout << "[Service] Registering: " << domain << ":" << instance << "\n";

    bool success = runtime->registerService(domain, instance, service);

    if (!success) {
        std::cerr << "[Service] ERROR: Failed to register service!\n";
        return 1;
    }

    std::cout << "[Service] Registered successfully!\n";

    service->startEventBroadcast();

    std::cout << "[Service] Running. Press Enter to stop...\n\n";
    std::cin.get();

    service->stopEventBroadcast();

    runtime->unregisterService(
        domain,
        GpuServiceImpl::StubInterface::getInterface(),
        instance
    );

    std::cout << "[Service] Stopped.\n";
    return 0;
}