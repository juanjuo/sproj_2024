//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>

class MainDeckGUI final : public juce::Component,
                          public DeckGUI

{
public:
  explicit MainDeckGUI(juce::ValueTree& valueTree)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195))
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


};




