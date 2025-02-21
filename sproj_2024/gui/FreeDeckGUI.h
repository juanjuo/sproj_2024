//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>
#include <DummyClip.h>
#include <Identifiers.h>

class FreeDeckGUI final : public juce::Component,
                          public DeckGUI,
                          public juce::DragAndDropContainer,
                          public  juce::DragAndDropTarget
{
public:
  explicit FreeDeckGUI(juce::ValueTree& tree)
    : DeckGUI(200, 160, juce::Colour::fromRGB(95, 95, 95)), valueTree(tree.getChildWithName(SP_ID::FREEDECK_BRANCH))
  {
    setSize(WINDOW_HEIGHT, WINDOW_HEIGHT);
    addAndMakeVisible(resizableEdge);
    resizableEdge.setAlwaysOnTop(true);
  }

  void createNewDummyClip()
  {
    juce::ValueTree newNode (SP_ID::CLIP);
    SP::createNewID(newNode);
    valueTree.appendChild(newNode, nullptr);
    addAndMakeVisible(new DummyClip(CLIP_WIDTH, CLIP_HEIGHT, newNode));
    //clips.items.add(clips.getLast()); //always returns right component?
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
  }

  void resized() override
  {
    const auto rect = getLocalBounds();
    resizableEdge.setBounds(rect.getX(), rect.getY(), rect.getWidth(), RESIZABLE_EDGE);
  }

  //Drag and drop methods

  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
    return true;
  }

  void itemDropped(const SourceDetails& dragSourceDetails) override
  {
    if (auto& component = dragSourceDetails.sourceComponent; isParentOf(component))
      component->setCentrePosition(dragSourceDetails.localPosition);
    // else
    // {
    //   addAndMakeVisible(component);
    //   dragSourceDetails.sourceComponent->setCentrePosition(dragSourceDetails.localPosition);
    // }
  }


private:
  juce::ResizableEdgeComponent resizableEdge{this, nullptr, juce::ResizableEdgeComponent::Edge::topEdge};

  //juce::OwnedArray<DummyClip> clips;

  int CLIP_WIDTH = 200;
  int CLIP_HEIGHT = 100;

  juce::ValueTree valueTree;
};




