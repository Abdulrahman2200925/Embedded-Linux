#ifndef SEVENSEGMENT_HPP
#define SEVENSEGMENT_HPP

#include <array>
#include "ISTREAM.hpp"
#include "OSTREAM.hpp"
#include "GPIO.hpp"

class SevenSegment : public istream, public ostream
{
public:
    // Constructor: SevenSegment HAS-A set of GPIO pins
    SevenSegment(GPIO&& a,
                 GPIO&& b,
                 GPIO&& c,
                 GPIO&& d,
                 GPIO&& e,
                 GPIO&& f,
                 GPIO&& g);

    // Implement ostream contract
    void writeDigit(int digit) override;

    // Output operator (correct semantic)
    SevenSegment& operator<<(int digit);

    // Utility
    void clear();

private:
    // Segment order: a b c d e f g
    std::array<GPIO, 7> segments;

    // Digit → segment lookup table
    static const bool digit_map[10][7];
};

#endif // SEVENSEGMENT_HPP
