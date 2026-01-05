#include "LogManager.hpp"
#include "LogMessage.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"

#include <memory>

int main() {
    LogManager manager;

    manager.addSink(std::make_shared<ConsoleSinkImpl>());
    manager.addSink(std::make_shared<FileSinkImpl>("log.txt"));

    
    LogMessage msg("MyApp", "Network", Severity::ERROR, "Connection failed");
    manager.log(msg);

    
    manager.log({"MyApp", "Database", Severity::INFO, "Connected"});

    manager.flush();

    return 0;
}
