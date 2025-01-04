//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>

class FreeDeckGUI final : public juce::Component,
                          public DeckGUI
{
public:

  explicit FreeDeckGUI(juce::ValueTree& valueTree)
    : DeckGUI(200, 200, juce::Colour::fromRGB(95,95,95))
  {
    setSize(WINDOW_HEIGHT, WINDOW_HEIGHT);
    addAndMakeVisible(resizableEdge);
  }

  void paint (juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
  }

  void resized() override
  {
    resizableEdge.setBounds(getLocalBounds());
  }

private:

  juce::ResizableEdgeComponent resizableEdge {this, nullptr, juce::ResizableEdgeComponent::Edge::topEdge};

};




