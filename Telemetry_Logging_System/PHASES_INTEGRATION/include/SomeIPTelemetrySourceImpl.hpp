#ifndef SomeIPTelemetrySourceImpl_HPP
#define SomeIPTelemetrySourceImpl_HPP

#include "ITelemetrySource.hpp"
#include <CommonAPI/CommonAPI.hpp>
#include <v1/omnimetron/gpu/GpuUsageDataProxy.hpp>
#include <string>
#include <mutex>

class SomeIPTelemetrySourceImpl : public ITelemetrySource {

public:

    static SomeIPTelemetrySourceImpl& getInstance();

    bool openSource() override;
    bool readSource(std::string& out) override;
    void subscribeToEvents();
     
    SomeIPTelemetrySourceImpl(const SomeIPTelemetrySourceImpl&)            = delete;
    SomeIPTelemetrySourceImpl& operator=(const SomeIPTelemetrySourceImpl&) = delete;
    SomeIPTelemetrySourceImpl(SomeIPTelemetrySourceImpl&&)                 = delete;
    SomeIPTelemetrySourceImpl& operator=(SomeIPTelemetrySourceImpl&&)      = delete;

    ~SomeIPTelemetrySourceImpl() = default;
      void requestShutdown() { shutdownRequested_ = true; }
private:

    SomeIPTelemetrySourceImpl() = default;

    // -------------------------------------------------------------------------
    // Use the default proxy type directly — NOT a template alias
    // GpuUsageDataProxyDefault = GpuUsageDataProxy<> (already instantiated)
    // We store it as shared_ptr to the default instantiation
    // -------------------------------------------------------------------------
    std::shared_ptr<v1::omnimetron::gpu::GpuUsageDataProxyDefault> proxy_;

    std::mutex  eventMutex_;
    float       latestEventValue_;
    bool        eventReceived_;
     std::atomic<bool> shutdownRequested_{false};
};

#endif