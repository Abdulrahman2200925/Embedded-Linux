#ifndef FileSinkImpl_hpp
#define FileSinkImpl_hpp

#include "ILogSink.hpp"
#include <string>
#include <fstream>

class FileSinkImpl : public ILogSink {
private:
    std::ofstream file;

public:
    FileSinkImpl(const std::string& filePath);
    
    void write(const LogMessage& message) override;
};

#endif