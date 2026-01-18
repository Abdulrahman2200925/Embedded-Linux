#include "FileSinkImpl.hpp"

FileSinkImpl::FileSinkImpl(const std::string& filePath) {
    file.open(filePath, std::ios::app);
}

void FileSinkImpl::write(const LogMessage& message) {
    file << message << std::endl;
}


