#ifndef OSTREAM_HPP
#define OSTREAM_HPP

#include "STREAM.hpp"

class ostream : public virtual Stream
{
public:
    // Output abstraction: write a single digit (0–9)
    virtual void writeDigit(int digit) = 0;

    // Virtual destructor already handled by Stream
};

#endif // OSTREAM_HPP
