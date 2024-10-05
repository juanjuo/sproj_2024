//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include <juce_core/juce_core.h>

class Clock : public juce::HighResolutionTimer
{
public:

  void hiResTimerCallback() override;

private:
};
