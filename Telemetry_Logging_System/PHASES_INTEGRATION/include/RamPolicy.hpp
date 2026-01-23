#ifndef RamPolicy_HPP
#define RamPolicy_HPP



#include "TelemetrySrc_enum.hpp"
#include "SeverityLvl_enum.hpp"
#include <iostream>
#include <string_view>

struct RamPolicy{


    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;

    static constexpr std::string_view unit = "%";

       static constexpr float WARNING=70.0f;
        static constexpr float CRITICAL=85.0f;

           // Logic to determine severity based on value
    static constexpr SeverityLvl_enum inferSeverity(float val) noexcept {
        return (val > CRITICAL) ? SeverityLvl_enum::CRITICAL
             : (val > WARNING)  ? SeverityLvl_enum::WARNING
             :                    SeverityLvl_enum::INFO;
    }


};









#endif