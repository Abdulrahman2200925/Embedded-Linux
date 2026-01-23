#include "LogSinkFactory.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include "SocketSinkImpl.hpp"  
#include <stdexcept>

std::shared_ptr<ILogSink> LogSinkFactory::create(
    LogSinkType_enum type,
    const std::string& param
) {
    switch(type) {
        case LogSinkType_enum::Console:
            return std::make_shared<ConsoleSinkImpl>();
            
        case LogSinkType_enum::File:
            if (param.empty()) {
                throw std::invalid_argument(
                    "FILE sink requires filename parameter"
                );
            }
            return std::make_shared<FileSinkImpl>(param);
            
        case LogSinkType_enum::Socket:
            if (param.empty()) {
                throw std::invalid_argument(
                    "SOCKET sink requires socket path parameter"
                );
            }
            return std::make_shared<SocketSinkImpl>(param);  
            
        default:
            throw std::invalid_argument("Unknown sink type");
    }
}