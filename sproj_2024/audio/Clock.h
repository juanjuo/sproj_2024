//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include <juce_core/juce_core.h>

class Clock : public juce::HighResolutionTimer
{
public:

    Clock() = default;

    void setNumerator(const int& numerator); //passing by reference since these values won't be modified

    void setDenominator(const int& denominator);

    void hiResTimerCallback() override;

    void setInterval();

    void setBPM(const int& bpm);

    void getMilliseconds(int milliseconds);

    float toMilliseconds(const int& bpm);

    float toBPM(const int& milliseconds);

private:
    int* bpm_val;
    int* numerator_val;
    int* denominator_val;

};

