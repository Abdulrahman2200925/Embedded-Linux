#include "SystemTelemetryWriter.hpp"
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <cstring>
#include <map>

SystemTelemetryWriter::SystemTelemetryWriter(const std::string& outputPath)
    : outputFile(outputPath, O_WRONLY | O_CREAT | O_TRUNC)
    , outputPath(outputPath)
{
}

bool SystemTelemetryWriter::isOpen() const {
    return outputFile.isValid();
}

// Parse /proc/stat and write CPU usage
bool SystemTelemetryWriter::writeCpuTelemetry() {
    if (!outputFile.isValid()) return false;
    
    // Read /proc/stat twice with delay to calculate usage
    auto readCpuTimes = []() -> std::pair<unsigned long long, unsigned long long> {
        std::ifstream file("/proc/stat");
        if (!file.is_open()) return {0, 0};
        
        std::string line;
        std::getline(file, line);
        
        std::istringstream ss(line);
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq;
        
        ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
        
        unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
        unsigned long long idleTime = idle + iowait;
        
        return {total, idleTime};
    };
    
    auto [total1, idle1] = readCpuTimes();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto [total2, idle2] = readCpuTimes();
    
    unsigned long long totalDelta = total2 - total1;
    unsigned long long idleDelta = idle2 - idle1;
    
    if (totalDelta == 0) return false;
    
    double cpuUsage = 100.0 * (1.0 - static_cast<double>(idleDelta) / totalDelta);
    
    // Format output
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed << "CPU_USAGE:" << cpuUsage << "%\n";
    
    std::string output = oss.str();
    ssize_t written = outputFile.write(output.c_str(), output.length());
    
    return written > 0;
}

// Parse /proc/meminfo and write memory usage
bool SystemTelemetryWriter::writeMemoryTelemetry() {
    if (!outputFile.isValid()) return false;
    
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return false;
    
    std::map<std::string, unsigned long> memInfo;
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string key;
        unsigned long value;
        
        ss >> key >> value;
        
        if (!key.empty() && key.back() == ':') {
            key.pop_back();
        }
        
        memInfo[key] = value;
    }
    
    unsigned long totalKB = memInfo["MemTotal"];
    unsigned long freeKB = memInfo["MemFree"];
    unsigned long availableKB = memInfo["MemAvailable"];
    unsigned long usedKB = totalKB - freeKB;
    
    double usedPercent = 100.0 * static_cast<double>(usedKB) / totalKB;
    double totalMB = totalKB / 1024.0;
    double usedMB = usedKB / 1024.0;
    
    // Format output
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed 
        << "MEMORY_USAGE:Total=" << totalMB << "MB,"
        << "Used=" << usedMB << "MB(" << usedPercent << "%)\n";
    
    std::string output = oss.str();
    ssize_t written = outputFile.write(output.c_str(), output.length());
    
    return written > 0;
}