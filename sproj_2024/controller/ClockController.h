//
// Created by Juan Diego on 10/9/24.
//

#pragma once

#include <../gui/ClockGUI.h>
#include <../audio/Clock.h>

class ClockController
{
public:
  ClockController(Clock clock, ClockGUI clockGUI);


private:
  ClockGUI clockGUI;
  Clock clock;

};