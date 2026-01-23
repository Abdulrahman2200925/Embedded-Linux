#ifndef GpuPolicy_HPP
#define GpuPolicy_HPP

#include "TelemetrySrc_enum.hpp"
#include "SeverityLvl_enum.hpp"
#include <string_view>

struct GpuPolicy{


    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::GPU;

    static constexpr std::string_view unit = "%";

       static constexpr float WARNING=80.0f;
        static constexpr float CRITICAL=95.0f;

           // Logic to determine severity based on value
    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }


};













#endif