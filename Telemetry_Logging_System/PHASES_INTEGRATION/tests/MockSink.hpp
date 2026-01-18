#ifndef MOCKSINK_H
#define MOCKSINK_H

#include "ILogSink.hpp"
#include <gmock/gmock.h>
#include <iostream>




class MockSink : public ILogSink{

public:
    MOCK_METHOD(void, write, (const LogMessage& message), (override));
    

};




















#endif