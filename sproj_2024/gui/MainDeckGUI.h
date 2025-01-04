//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class MainDeckGUI final : public juce::Component
{
public:
  explicit MainDeckGUI(juce::ValueTree& valueTree)
  {
  }

  void paint (juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
  }

  void resized() override
  {
  }
private:

  const juce::Colour BACKGROUND_COLOUR = juce::Colour::fromRGB(195, 195, 195);
  const int BORDER_WIDTH = 2;

};




