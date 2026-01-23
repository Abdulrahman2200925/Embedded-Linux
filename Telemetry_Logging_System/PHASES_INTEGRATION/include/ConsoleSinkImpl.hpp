#ifndef ConsoleSinkImpl_hpp
#define ConsoleSinkImpl_hpp

#include "ILogSink.hpp"


class ConsoleSinkImpl :public ILogSink {


public :

void write(const LogMessage& message) override;

};




















#endif