//
// Created by Juan Diego on 12/31/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class DeckGUI
{
protected:
  DeckGUI(const int width, const int height, const juce::Colour background)
    : WINDOW_WIDTH(width), WINDOW_HEIGHT(height), BACKGROUND_COLOUR(background)
  {
  }

  const int WINDOW_WIDTH;
  const int WINDOW_HEIGHT;
  const juce::Colour BACKGROUND_COLOUR;
  const int BORDER_WIDTH = 2;
};

