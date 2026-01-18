
#ifndef LogManager_HPP
#define LogManager_HPP

#include <vector>
#include "ILogSink.hpp"
#include <memory>





class LogManager{
private:
std::vector<LogMessage> buffer;
std::vector<std::shared_ptr<ILogSink>> sinks;


public:
void addSink(std::shared_ptr<ILogSink> sink);
void log(const LogMessage& message);
void flush();



};




#endif