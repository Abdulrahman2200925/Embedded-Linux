#ifndef SystemTelemetryWriter_HPP
#define SystemTelemetryWriter_HPP

#include "SafeFile.hpp"
#include <string>

class SystemTelemetryWriter {
private:
    SafeFile outputFile;
    std::string outputPath;
    
public:
    // Constructor - opens output file
    SystemTelemetryWriter(const std::string& outputPath);
    
    // Write CPU telemetry from /proc/stat
    bool writeCpuTelemetry();
    
    // Write Memory telemetry from /proc/meminfo
    bool writeMemoryTelemetry();
    
    // Check if writer is ready
    bool isOpen() const;
};

#endif