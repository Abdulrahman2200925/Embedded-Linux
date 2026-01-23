#ifndef CpuPolicy_HPP
#define CpuPolicy_HPP

#include "TelemetrySrc_enum.hpp"
#include "SeverityLvl_enum.hpp"
#include <string_view>





struct CpuPolicy {
    // What type of telemetry is this?
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
    
    // What unit are we measuring?
    static constexpr std::string_view unit = "%";
    
    // Threshold values (compile-time constants)
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
    
    // Logic to determine severity based on value
    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }
};

























#endif