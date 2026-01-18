#ifndef FileTelemetrySourceImpl_HPP
#define FileTelemetrySourceImpl_HPP
#include "SafeFile.hpp"
#include "ITelemetrySource.hpp"

class FileTelemetrySourceImpl:public ITelemetrySource{
private:
 SafeFile file;
 std::string filepath;

 public:
 FileTelemetrySourceImpl(const std::string& path);
  bool openSource() override;
  bool readSource(std::string& out) override;
};





#endif