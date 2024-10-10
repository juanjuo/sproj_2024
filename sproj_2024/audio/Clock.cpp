//
// Created by Juan Diego on 9/30/24.
//

#include "Clock.h"

void Clock::setNumerator(const int& numerator)
{
    *numerator_val = numerator;
}

void Clock::setDenominator(const int& denominator)
{
    *denominator_val = denominator;
}

void Clock::hiResTimerCallback()
{
    std::cout << "cycle" << std::endl;
}

void Clock::setInterval()
{

}

void Clock::setBPM(const int& bpm)
{
    *bpm_val = bpm;
}

void Clock::getMilliseconds(int milliseconds)
{

}

float Clock::toMilliseconds(const int& bpm)
{
    if (bpm <= 0) {
        std::cerr << "BPM must be a positive integer." << std::endl;
        return -1; // Indicate an error
    }
    return 60000.0 / bpm; // Convert BPM to milliseconds
}

float Clock::toBPM(const int& milliseconds)
{
    if (milliseconds <= 0) {
        std::cerr << "Milliseconds must be a positive value." << std::endl;
        return -1; // Indicate an error
    }
    return 60000.0 / milliseconds; // Convert milliseconds to BPM
}






