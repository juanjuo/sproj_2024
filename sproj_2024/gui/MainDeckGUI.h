//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>
#include <DummyClip.h>

//this is responsible for showing and organizing a grid of draggable components, with the height of a track
class MainDeckTrack final : public juce::Component,
                            public DeckGUI,
                            public juce::DragAndDropContainer,
                            public juce::DragAndDropTarget
{
public:
  //represents every individual tile in the grid where audio files can be positioned
  class MainDeckTile final : public juce::Component
  {
  public:
    MainDeckTile(int width, int height)
    {
      //setEnabled(false);
      setAlwaysOnTop(true);
      setSize(width, height);
      //setAlpha(1);
    }

    void paint(juce::Graphics& g) override
    {
      //g.fillAll(juce::Colours::transparentW);
      g.setColour(juce::Colours::black);
      g.drawRect(this->getLocalBounds(), 1);
    }

    void resized() override
    {
    }

    // juce::Point<int> calculatePosition()
    // {
    //   return getParentComponent()->getLocalPoint(this, getPosition());
    // }

  private:
    int position{}; //represents how many beats until playback starts. (maybe this number must be in ValueTree

    //DummyClip clip;
  };

  MainDeckTrack(int width, int height): DeckGUI(width, height, juce::Colour::fromRGB(195, 195, 195))
  {
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setUpGrid();
    addTiles(NUM_TILES); //add all tiles
  }

  void addTiles(const int numOfTiles)
  {
    for (int i = 0; i < numOfTiles; i++)
    {
      // auto* tile = new MainDeckTile(TILE_WIDTH, TILE_HEIGHT);
      // tile->toBehind(this);
      // addAndMakeVisible(tiles.add(tile));
      //grid.items.add(tiles.getLast()); //always returns right component?

      auto* tile = new MainDeckTile(TILE_WIDTH, TILE_HEIGHT);
      grid.items.add(tile);
      addAndMakeVisible(tile);
    }
  }

  void setUpGrid()
  {
    using Track = juce::Grid::TrackInfo;
    grid.rowGap = juce::Grid::Px(0);
    grid.columnGap = juce::Grid::Px(0);

    grid.templateRows = {Track(juce::Grid::Px(TILE_HEIGHT))};

    grid.templateColumns = {Track(juce::Grid::Fr(NUM_TILES))};

    grid.autoColumns = Track(juce::Grid::Fr(NUM_TILES));
    grid.autoRows = Track(juce::Grid::Px(TILE_HEIGHT));

    //grid.justifyContent = juce::Grid::JustifyContent::stretch; //technically not necessary?
    grid.autoFlow = juce::Grid::AutoFlow::column;
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH - 1);
    // if (!tiles.isEmpty())
    //   grid.performLayout(getLocalBounds());

    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    setUpGrid();
  }

  //Drag and drop methods

  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
    return true;
  }

  void itemDropped(const SourceDetails& dragSourceDetails) override //is there better ways of doing this??
  {
    // const juce::WeakReference<Component> child = dragSourceDetails.sourceComponent; //
    // const auto tile = static_cast<MainDeckTile*>(child.get());
    // if ( tile != nullptr)
    // {
    //   const int index = getIndexOfChildComponent(tile); //didn't work
    //   std::cout << index << std::endl;
    // }

    // auto point = dragSourceDetails.localPosition;
    // auto tiles = getChildren();
    // for (int i = 0; i < tiles.size(); i++)
    // {
    //   if (tiles[i]->hitTest(point.x, point.y))
    //     std::cout << i << std::endl;
    // }

    auto clip = dragSourceDetails.sourceComponent;
    auto point = dragSourceDetails.localPosition; //make better type checking here
    auto component = getComponentAt(point);
    if (component != nullptr)
    {
      clip->setTopLeftPosition(component->getPosition());
      addAndMakeVisible(clip);
      const int index = getIndexOfChildComponent(component);
      std::cout << index << std::endl;
    }

    // auto tile = dragSourceDetails.sourceComponent;
    // const int index = getIndexOfChildComponent(tile.get());
    // std::cout << index << std::endl;
  }

private:
  juce::Grid grid;

  int TILE_WIDTH = 20;
  int TILE_HEIGHT = 100;

  int NUM_TILES = 32;

  //juce::OwnedArray<MainDeckTile> tiles; //array of tiles in this track
};


class MainDeckGUI final : public juce::Component,
                          public DeckGUI
{
public:
  explicit MainDeckGUI(juce::ValueTree& valueTree)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195))
  {
    setUpGrid(TRACK_WIDTH);
    //TRACK_WIDTH = getWidth(); //maybe?
  }

  void addTrack()
  {
    addAndMakeVisible(mainDeckTracks.add(new MainDeckTrack(TRACK_WIDTH, TRACK_WIDTH)));
    grid.items.add(mainDeckTracks.getLast()); //always returns right component?
  }

  void setUpGrid(const int width)
  {
    using Track = juce::Grid::TrackInfo;
    grid.rowGap = juce::Grid::Px(0);
    grid.columnGap = juce::Grid::Px(0);

    grid.templateRows = {Track(juce::Grid::Px(TRACK_HEIGHT))};

    grid.templateColumns = {Track(juce::Grid::Px(width))};

    grid.autoColumns = Track(juce::Grid::Px(TRACK_WIDTH));
    grid.autoRows = Track(juce::Grid::Px(TRACK_HEIGHT));

    //grid.justifyContent = juce::Grid::JustifyContent::stretch; //technically not necessary?
    grid.autoFlow = juce::Grid::AutoFlow::row;
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
    if (!mainDeckTracks.isEmpty())
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    setUpGrid(getWidth());
  }

private:
  juce::Grid grid;

  juce::OwnedArray<MainDeckTrack> mainDeckTracks; //array of tracks in MainDeck

  int TRACK_WIDTH = 200;
  int TRACK_HEIGHT = 100;
};




