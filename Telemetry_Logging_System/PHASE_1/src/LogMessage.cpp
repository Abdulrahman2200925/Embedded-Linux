#include "LogMessage.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string severityToString(Severity severity) {
    switch (severity) {
        case Severity::DEBUG:    return "DEBUG";
        case Severity::INFO:     return "INFO";
        case Severity::WARNING:  return "WARNING";
        case Severity::ERROR:    return "ERROR";
        case Severity::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

LogMessage::LogMessage(const std::string& appName,
                       const std::string& context,
                       Severity severity,
                       const std::string& text)
    : appName(appName)
    , context(context)
    , severity(severity)
    , text(text)
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    timeStamp = ss.str();
}

std::ostream& operator<<(std::ostream& os, const LogMessage& logMessage) {
    os << "[" << logMessage.appName << "],"
       << "[" << logMessage.timeStamp << "],"
       << "[" << logMessage.context << "],"
       << "[" << severityToString(logMessage.severity) << "],"  
       << "[" << logMessage.text << "]";
    return os;
}
