#ifndef ISTREAM_HPP
#define ISTREAM_HPP

#include "STREAM.hpp"


class istream : public virtual Stream
{
public:
    // Read a single valid digit (0–9) from terminal
    int readDigit();
 
};

#endif // ISTREAM_HPP
