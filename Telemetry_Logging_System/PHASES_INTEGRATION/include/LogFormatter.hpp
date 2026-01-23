#ifndef LogFormatter_hpp
#define LogFormatter_hpp

#include "LogMessage.hpp"
#include "SeverityLvl_enum.hpp"
#include "TelemetrySrc_enum.hpp"
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <stdexcept>

template<typename Policy>
class LogFormatter {
  private:
    std::string msgDescription(float val);
    std::string telemetrySourceToString(TelemetrySrc_enum src);
    Severity convertToPhase1Severity(SeverityLvl_enum phase3_sev);


public:
    std::optional<LogMessage> formatDataToLogMsg(const std::string& raw);


};





template<typename Policy>
std::string LogFormatter<Policy>::telemetrySourceToString(TelemetrySrc_enum src) {
    switch(src) {
        case TelemetrySrc_enum::CPU:
            return "CPU";
        case TelemetrySrc_enum::GPU:
            return "GPU";
        case TelemetrySrc_enum::RAM:
            return "RAM";
        default:
            return "UNKNOWN";
    }
}

template<typename Policy>
Severity LogFormatter<Policy>::convertToPhase1Severity(SeverityLvl_enum phase3_sev) {
    switch(phase3_sev) {
        case SeverityLvl_enum::CRITICAL:
            return Severity::CRITICAL;
        case SeverityLvl_enum::WARNING:
            return Severity::WARNING;
        case SeverityLvl_enum::INFO:
            return Severity::INFO;
        default:
            return Severity::INFO;
    }
}



template<typename Policy>
std::string LogFormatter<Policy>::msgDescription(float val){
  
 std::ostringstream ss;
 ss<<std::fixed<<std::setprecision(2);


 std::string source_name =telemetrySourceToString(Policy::context);
 std::string_view unit=Policy::unit;

 ss << source_name << " usage at " << val << unit;
    
    return ss.str();


}


template<typename Policy>
std::optional<LogMessage> LogFormatter<Policy>::formatDataToLogMsg(const std::string& raw) {
    // Step 1: Parse string to float
    float value;
    try {
        value = std::stof(raw);
    } catch (const std::exception&) {
        return std::nullopt;
    }
    
    // Step 2: Infer severity using Policy
    SeverityLvl_enum phase3_severity = Policy::inferSeverity(value);
    
    // Step 3: Convert to Phase 1 severity
    Severity phase1_severity = convertToPhase1Severity(phase3_severity);
    
    // Step 4: Generate message components
    std::string description = msgDescription(value);
    std::string appName = telemetrySourceToString(Policy::context);
    
    // Step 5: Create LogMessage
    LogMessage msg(
        appName,
        "Telemetry",
        phase1_severity,
        description
    );
    
    return msg;
}





#endif