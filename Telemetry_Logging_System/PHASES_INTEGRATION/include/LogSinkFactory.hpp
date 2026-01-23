#ifndef LogSinkFactory_HPP
#define LogSinkFactory_HPP

#include "ILogSink.hpp"
#include "LogSinkType_enum.hpp"
#include <memory>
#include <string>

class LogSinkFactory {
public:
    static std::shared_ptr<ILogSink> create(
        LogSinkType_enum type,
        const std::string& param = ""
    );
    
    // Delete constructors - static-only class
    LogSinkFactory() = delete;
    LogSinkFactory(const LogSinkFactory&) = delete;
    LogSinkFactory& operator=(const LogSinkFactory&) = delete;
};

#endif