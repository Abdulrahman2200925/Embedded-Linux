#include <iostream>
#include "ISTREAM.hpp"

int istream::readDigit()
   {
        char c;

        while (true) {
            std::cin >> c;                 // read from terminal

            if (c >= '0' && c <= '9') {
                return c - '0';             // convert char to int
            }

            // invalid input is ignored gracefully
        }
    }