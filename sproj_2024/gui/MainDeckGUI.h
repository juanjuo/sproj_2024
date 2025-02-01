//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>


class MainDeckTrack : public juce::Component,
                      public DeckGUI
{
public:
  MainDeckTrack(int width, int height): DeckGUI(width, height, juce::Colour::fromRGB(144, 144, 144))
  {
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  ~MainDeckTrack()
  {

  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
  }

  void resized() override
  {
  }

private:
};


class MainDeckGUI final : public juce::Component,
                          public DeckGUI
{
public:
  explicit MainDeckGUI(juce::ValueTree& valueTree)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195))
  {
    setUpGrid(TRACK_WIDTH);
  }

  void addTrack()
  {
    addAndMakeVisible(mainDeckTracks.add(new MainDeckTrack (TRACK_WIDTH, TRACK_WIDTH)));
    grid.items.add(mainDeckTracks.getLast()); //always returns right component?
  }

  void setUpGrid(const int width)
  {
    using Track = juce::Grid::TrackInfo;
    grid.rowGap    = juce::Grid::Px(0);
    grid.columnGap = juce::Grid::Px(0);

    grid.templateRows = { Track (juce::Grid::Px(TRACK_HEIGHT)) };

    grid.templateColumns = { Track (juce::Grid::Px(width)) };

    grid.autoColumns = Track (juce::Grid::Px(TRACK_WIDTH));
    grid.autoRows    = Track (juce::Grid::Px(TRACK_HEIGHT));

    //grid.justifyContent = juce::Grid::JustifyContent::stretch; //technically not necessary?
    grid.autoFlow = juce::Grid::AutoFlow::row;
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
  }

  void resized() override
  {
  }

private:
  juce::Grid grid;

  juce::OwnedArray<MainDeckTrack> mainDeckTracks;

  int TRACK_WIDTH = 200;
  int TRACK_HEIGHT = 100;
};




