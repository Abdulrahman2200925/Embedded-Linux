#ifndef LOGMESSAGE_HPP
#define LOGMESSAGE_HPP

#include <string>
#include <iosfwd>

enum class Severity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Convert Severity to string
std::string severityToString(Severity severity);

class LogMessage {
private:
    std::string appName;
    std::string context;
    std::string timeStamp;
    Severity severity;
    std::string text;

public:
    // Constructor
    LogMessage(const std::string& appName,
               const std::string& context,
               Severity severity,
               const std::string& text);
    
    // Special member functions - Rule of Zero (explicitly defaulted)
    ~LogMessage() = default;
    LogMessage(const LogMessage&) = default;
    LogMessage& operator=(const LogMessage&) = default;
    LogMessage(LogMessage&&) = default;
    LogMessage& operator=(LogMessage&&) = default;
    
    // Getters
    const std::string& getAppName() const { return appName; }
    const std::string& getContext() const { return context; }
    const std::string& getTimeStamp() const { return timeStamp; }
    Severity getSeverity() const { return severity; }
    const std::string& getText() const { return text; }
    
    // Friend function for output
    friend std::ostream& operator<<(std::ostream& os, const LogMessage& logMessage);
};

#endif /* LOGMESSAGE_HPP */
