#include "GpuServiceImpl.hpp"
#include <iostream>
#include <fstream>

GpuServiceImpl::GpuServiceImpl()
    : broadcasting_(false)
{
    std::cout << "[GpuService] Created\n";
}

GpuServiceImpl::~GpuServiceImpl() {
    stopEventBroadcast();
}

void GpuServiceImpl::requestGpuUsageData(
    const std::shared_ptr<CommonAPI::ClientId> _client,
    requestGpuUsageDataReply_t _reply)
{
    float temp = readGpuUsage();
    std::cout << "[GpuService] Request received → replying with " << temp << "°C\n";
    _reply(temp);
}

void GpuServiceImpl::startEventBroadcast() {
    broadcasting_.store(true);
    broadcastThread_ = std::thread(&GpuServiceImpl::broadcastLoop, this);
    std::cout << "[GpuService] Event broadcast started\n";
}

void GpuServiceImpl::stopEventBroadcast() {
    if (broadcasting_.load()) {
        broadcasting_.store(false);
        if (broadcastThread_.joinable())
            broadcastThread_.join();
        std::cout << "[GpuService] Event broadcast stopped\n";
    }
}

void GpuServiceImpl::broadcastLoop() {
    while (broadcasting_.load()) {
        float temp = readGpuUsage();
        std::cout << "[GpuService] Firing event → " << temp << "°C\n";
        fireNotifyGpuUsageDataChangeEvent(temp);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

float GpuServiceImpl::readGpuUsage() {
    // Read CPU/GPU temperature from RPi3 thermal sensor
    // /sys/class/thermal/thermal_zone0/temp gives millidegrees Celsius
    std::ifstream file("/sys/class/thermal/thermal_zone0/temp");
    if (!file.is_open()) {
        std::cerr << "[GpuService] WARNING: Cannot read temperature sensor, returning 0.0\n";
        return 0.0f;
    }

    int millidegrees = 0;
    file >> millidegrees;

    // Convert millidegrees to degrees Celsius
    float celsius = millidegrees / 1000.0f;

    std::cout << "[GpuService] Temperature read: " << celsius << "°C\n";
    return celsius;
}