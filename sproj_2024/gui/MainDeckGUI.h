//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <DeckGUI.h>
#include <DummyClip.h>
#include <Identifiers.h>

class MainDeckTile final : public juce::Component
{
public:
  MainDeckTile(int width, int height, int p)
  {
    setAlwaysOnTop(true);
    setSize(width, height);
    position = p;
  }

  void paint(juce::Graphics& g) override
  {
    g.setColour(selectedColour);
    g.fillRect(getLocalBounds());
    g.setColour(juce::Colours::black);
    g.drawRect(this->getLocalBounds(), 1);
  }

  void resized() override
  {
  }

  int getTilePosition() const
  {
    return position;
  }

private:
  int position{}; //represents how many beats until playback starts. (maybe this number must be in ValueTree

  //juce::Colour selectedColour = juce::Colours::green;

public:
  juce::Colour selectedColour = juce::Colour::fromRGBA(0, 0, 0, 0);
};

//class for changing the color of the tiles when selected / deselected. tile color not working
class SPSelectedItemSet final : public juce::SelectedItemSet<MainDeckTile*>
{
public:
  SPSelectedItemSet()
  {

  }

  // void itemSelected(MainDeckTile* tile) override
  // {
  //   tile->selectedColour = juce::Colour::fromRGB(51, 122, 235);
  // }
  //
  // void itemDeselected(MainDeckTile* tile) override
  // {
  //   tile->selectedColour = juce::Colour::fromRGBA(0, 0, 0, 0);
  // }
};

//this is responsible for showing and organizing a grid of draggable components, with the height of a track
class MainDeckTrack final : public juce::Component,
                            public DeckGUI,
                            public juce::DragAndDropContainer,
                            public juce::LassoSource<MainDeckTile*>
                            //public juce::ChangeListener
{
public:

  //represents every individual tile in the grid where audio files can be positioned
  MainDeckTrack(int width, int height, juce::ValueTree& n):
    DeckGUI(width, height, juce::Colour::fromRGB(195, 195, 195)), node(n)
  {
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setUpGrid();
    addTiles(NUM_TILES); //add all tiles
  }

  void addTiles(const int numOfTiles)
  {
    for (int i = 0; i < numOfTiles; i++)
    {

      auto* tile = new MainDeckTile(TILE_WIDTH, TILE_HEIGHT, i);
      grid.items.add(tile);
      tileList.add(tile);
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

  //for mask
  void trackAppendClipValueTree(const juce::ValueTree& tree)
  {
    node.appendChild(tree, nullptr);
  }

  void paint(juce::Graphics& g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH - 1);

    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    //setUpGrid();
  }

  void childrenChanged() override //for indexing bug
  {
    numOfClips++;
  }

  // LassoSource methods
  void findLassoItemsInArea(juce::Array<MainDeckTile*>& itemsFound, const juce::Rectangle<int>& area) override
  {
    for (auto element : tileList)
    {
      // std::cout << element->getLocalBounds().toString() << std::endl;
      // std::cout << area.toString() << std::endl;
      if (area.intersects(element->getBounds()))
      {
        itemsFound.add(element);
      }
    }
  }

  juce::SelectedItemSet<MainDeckTile*>& getLassoSelection() override
  {
    return tilesSelected;
  }

  // void resetTiles() //tile color not working
  // {
  //   for (auto tile : tileList)
  //   {
  //     tile->selectedColour = juce::Colour::fromRGBA(0, 0, 0, 0);
  //   }
  // }

private:
  juce::Grid grid;

  juce::ValueTree node;

  juce::Array<MainDeckTile*> tileList;


  int TILE_WIDTH = 20;
  int TILE_HEIGHT = 100;

  int NUM_TILES = 32;

  int numOfClips = -NUM_TILES + 1; //to fix issue with indexing of children //USE TILE LIST

public:
  SPSelectedItemSet tilesSelected; //list for all tiles
};


class MainDeckMask final : public juce::Component,
                           public juce::DragAndDropTarget
{
public:

  bool isDragging = false;

  explicit MainDeckMask(MainDeckTrack* t) : track(t)
  {
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoFillColourId, juce::Colours::transparentWhite);
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoOutlineColourId, juce::Colours::transparentWhite);
    setAlwaysOnTop(true);
    setBounds(track->getBounds());
  }

  void resized() override
  {
    setBounds(track->getBounds());
  }

  void mouseDown(const juce::MouseEvent& event) override
  {
    //track->resetTiles(); //tile color not working
    setBounds(track->getBounds()); //BUG!! size of mask is not dynamically resizing
    addAndMakeVisible(lassoComponent);
    lassoComponent.beginLasso(event, track);
  }

  void mouseDrag(const juce::MouseEvent& event) override
  {
    lassoComponent.dragLasso(event);
  }

  void mouseUp(const juce::MouseEvent& event) override
  {
    lassoComponent.endLasso();
    removeChildComponent(&lassoComponent);
    for (auto tile : track->tilesSelected)
    {
      std::cout << tile->getTilePosition() << std::endl;
    }
  }

  //Drag and drop methods

  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
    return true;
  }

  void itemDropped(const SourceDetails& dragSourceDetails) override //is there better ways of doing this??
  {

    auto clip = dragSourceDetails.sourceComponent;
    auto point = dragSourceDetails.localPosition; //make better type checking here
    auto tile = track->getComponentAt(point);
    if (tile != nullptr) //move component
    {
      clip->setTopLeftPosition(tile->getPosition());
      track->addAndMakeVisible(clip);
      const int index = track->getIndexOfChildComponent(tile);// - numOfClips;
      //the order of children in the children array changes when you add a new clip //USE TILE LIST
      std::cout << index << std::endl;

      if (auto dummyClip = dynamic_cast<DummyClip*>(clip.get()); dummyClip != nullptr) //move ValueTree
      {
        auto tree = dummyClip->getValueTree();
        tree.setProperty(SP_ID::clip_start_value, index, nullptr); //add starting value
        tree.getParent().removeChild(tree, nullptr);
        track->trackAppendClipValueTree(tree);
      }
    }
  }

private:
  MainDeckTrack* track;

  juce::LassoComponent<MainDeckTile*> lassoComponent;
};

class MainDeckGUI final : public juce::Component,
                          public DeckGUI
{
public:
  explicit MainDeckGUI(const juce::ValueTree& v)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195)), valueTree(v)
  {
    setUpGrid(TRACK_WIDTH);
    //TRACK_WIDTH = getWidth(); //maybe?
  }

  void addTrack(juce::ValueTree& newNode)
  {
    auto* track = new MainDeckTrack(TRACK_WIDTH, TRACK_WIDTH, newNode);
    auto* mask = new MainDeckMask(track);
    addAndMakeVisible(track);
    addAndMakeVisible(mask);
    grid.items.add(track); //always returns right component?
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
    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    setUpGrid(getWidth());
  }

private:
  juce::Grid grid;

  int TRACK_WIDTH = 1500;
  int TRACK_HEIGHT = 100;

  juce::ValueTree valueTree;
};




