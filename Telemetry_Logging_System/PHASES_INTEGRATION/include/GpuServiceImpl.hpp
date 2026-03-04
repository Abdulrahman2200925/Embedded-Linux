#ifndef GpuServiceImpl_HPP
#define GpuServiceImpl_HPP

// =============================================================================
// GpuServiceImpl — inherits from GpuUsageDataStubDefault
// StubDefault already implements initStubAdapter() internally
// We only need to override requestGpuUsageData()
// =============================================================================

#include <v1/omnimetron/gpu/GpuUsageDataStubDefault.hpp>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

class GpuServiceImpl : public v1::omnimetron::gpu::GpuUsageDataStubDefault {

public:
    GpuServiceImpl();
    ~GpuServiceImpl();

    // Called when client sends a request
    void requestGpuUsageData(
        const std::shared_ptr<CommonAPI::ClientId> _client,
        requestGpuUsageDataReply_t _reply
    ) override;

    // Event broadcasting
    void startEventBroadcast();
    void stopEventBroadcast();

private:
    void  broadcastLoop();
    float readGpuUsage();

    std::thread       broadcastThread_;
    std::atomic<bool> broadcasting_;
};

#endif