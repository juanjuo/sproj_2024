//
// Created by Juan Diego on 2/1/25.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>

class RulerDeckGUI final : public juce::Component,
                          public DeckGUI
{
public:

    explicit RulerDeckGUI()
      : DeckGUI(200, 200, juce::Colour::fromRGB(15,15,15))
    {
        setSize(WINDOW_HEIGHT, WINDOW_HEIGHT);
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

