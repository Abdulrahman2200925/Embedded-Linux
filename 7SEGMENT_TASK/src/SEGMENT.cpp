#include "SEGMENT.hpp"
#include <utility>   // for std::move


// a  b  c  d  e  f  g
const bool SevenSegment::digit_map[10][7] =
{
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

SevenSegment::SevenSegment(GPIO&& a,
                           GPIO&& b,
                           GPIO&& c,
                           GPIO&& d,
                           GPIO&& e,
                           GPIO&& f,
                           GPIO&& g)
    : segments{ std::move(a), std::move(b), std::move(c),
                std::move(d), std::move(e), std::move(f),
                std::move(g) }
{
}

void SevenSegment::writeDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    for (int i = 0; i < 7; ++i) {
        segments[i].write_value(digit_map[digit][i]);
    }
}
SevenSegment& SevenSegment::operator<<(int digit)
{
    writeDigit(digit);
    return *this;
}

void SevenSegment::clear()
{
    for (auto& seg : segments) {
        seg.write_value(false);
    }
}
