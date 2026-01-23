 
#include "ConsoleSinkImpl.hpp"
#include "LogMessage.hpp"

#include <iostream>


void ConsoleSinkImpl::write(const LogMessage& message) {


std::cout << message << std::endl;




}
