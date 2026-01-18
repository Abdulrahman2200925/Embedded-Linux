#include "LogManager.hpp"

void LogManager::addSink(std::shared_ptr<ILogSink> sink){
    sinks.push_back(sink);
}

void LogManager::log(const LogMessage& message){
    buffer.push_back(message);
}

void LogManager::flush(){
    for(auto& sink : sinks){
        for(auto& message : buffer){
            sink->write(message);
        }
    }
    buffer.clear();
}