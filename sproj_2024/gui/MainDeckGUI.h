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
  MainDeckTile(int width, int height, const int p) : position(p)
  {
    setAlwaysOnTop(true);
    setSize(width, height);
  }

  void setOccupied(const bool active)
  {
    isOccupied = active;
  }

  bool getOccupied()
  {
    return isOccupied;
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
  int position; //represents how many beats until playback starts. (maybe this number must be in ValueTree

  bool isOccupied = false; //Active = can be assigned a clip

  //juce::Colour selectedColour = juce::Colours::green

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
  MainDeckTrack(int width, int height, juce::ValueTree& n, juce::ValueTree freeDeck, FreeDeckGUI& fdeck):
    DeckGUI(width, height, juce::Colour::fromRGB(195, 195, 195)), node(n), freeDeckNode(freeDeck), freeDeck(fdeck)
  {
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setUpGrid();
    addTiles(NUM_TILES); //add all tiles
  }

  void makeTilesOccupied(int start, int end) const
  {
    for (int i = start; i < end; i++)
    {
      tileList[i]->setOccupied(true);
    }
  }

  bool areTilesOccupied(int start, int end) const
  {
    bool canPerform = true;
    for (int i = start; i < end; i++)
    {
      if (tileList[i]->getOccupied())
        canPerform = false;
    }
    return canPerform;
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

  void createNewClipForTrack()
  {
    const int numOfBeats = tilesSelected.getNumSelected();
    if (numOfBeats > 0)
    {
      int totalWidth = 0; //total width of tiles to create the new clip with
      auto minTile = tilesSelected.getSelectedItem(0); //tile with the lowest starting point
      auto maxTile = tilesSelected.getSelectedItem(numOfBeats - 1);
      for (auto tile : tilesSelected)
      {
        totalWidth = totalWidth + tile->getWidth();
        if (minTile->getTilePosition() > tile->getTilePosition())
          minTile = tile; //look for lowest starting point, since this is the point where the clip will be created
        if (maxTile->getTilePosition() < tile->getTilePosition())
          maxTile = tile; //look for highest starting point, since this is the point where the clip will end
      }
      const auto start = minTile->getTilePosition();
      const auto end = maxTile->getTilePosition() + 1;
      if (areTilesOccupied(start, end))
      {
        makeTilesOccupied(start, end);

        //setup ValueTree
        juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
        juce::ValueTree deckClipNode(SP_ID::CLIP); //add ValueTree to freedeck
        SP::createNewID(trackClipNode);
        deckClipNode.setProperty(SP_ID::U_ID, trackClipNode.getProperty(SP_ID::U_ID), nullptr);
        freeDeckNode.appendChild(deckClipNode, nullptr);
        //std::cout << "position: " << minTile->getTilePosition() << std::endl;
        node.appendChild(trackClipNode, nullptr);

        //create components
        std::cout << "numOfBeats: " << numOfBeats << std::endl;
        const auto point = minTile->getPosition();
        juce::Colour colour = juce::Colour::fromRGB(rand() % 255, rand() % 255, rand() % 255);
        auto trackClip = new DummyClip(totalWidth, TILE_HEIGHT, point, trackClipNode, colour);
        trackClip->setUpValueTree(start, end, "dummyClip");
        addAndMakeVisible(trackClip);
        deckClipNode.setProperty(SP_ID::clip_length_value, end - start, nullptr); //set up length
        freeDeck.addAndMakeVisible(new DummyClip(totalWidth, TILE_HEIGHT, freeDeck.getLocalBounds(), deckClipNode,
                                                 colour)); //add component to freeDeck
      }
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

  juce::ValueTree node; //this track's value tree

  juce::ValueTree freeDeckNode; //freedeckgui value tree branch

  SPSelectedItemSet tilesSelected; //list for all tiles

  FreeDeckGUI& freeDeck;


  int TILE_WIDTH = 20;
  int TILE_HEIGHT = 100;

  int NUM_TILES = 32;

  int numOfClips = -NUM_TILES + 1; //to fix issue with indexing of children //USE TILE LIST

public:
  juce::Array<MainDeckTile*> tileList;
};


class MainDeckMask final : public juce::Component,
                           public juce::DragAndDropTarget
{
public:
  bool isDragging = false;

  explicit MainDeckMask(MainDeckTrack* t) : track(t)
  {
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoFillColourId,
                             juce::Colours::transparentWhite);
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoOutlineColourId,
                             juce::Colours::transparentWhite);
    setAlwaysOnTop(true);
    setBounds(track->getBounds());
  }

  // bool canAddTrack(auto list) //not super safe
  // {
  //   bool canPerform = true;
  //   for (auto tile : list)
  //   {
  //     if (tile->getOccupied())
  //       canPerform = false;
  //   }
  //   return canPerform;
  // }

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
    auto list = track->getLassoSelection();
    track->createNewClipForTrack();
    lassoComponent.endLasso();
    removeChildComponent(&lassoComponent);
  }

  //Drag and drop methods

  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
    return true;
  }

  void itemDropped(const SourceDetails& dragSourceDetails) override //is there better ways of doing this??
  {
    auto* dummyClip = dynamic_cast<DummyClip*>(dragSourceDetails.sourceComponent.get());
    if (dummyClip != nullptr)
    {
      auto point = dragSourceDetails.localPosition;
      auto tile = dynamic_cast<MainDeckTile*>(track->getComponentAt(point)); //make better type checking here
      if (tile != nullptr) //move component
      {
        const int start = track->tileList.indexOf(tile); // - numOfClips;
        //the order of children in the children array changes when you add a new clip //USE TILE LIST
        bool canPerform = true;
        int length = dummyClip->getValueTree().getProperty(SP_ID::clip_length_value);
        int end = start + length;
        if (track->areTilesOccupied(start, end))
        {
          track->makeTilesOccupied(start, end - 1);
          std::cout << start << std::endl; //add new track
          juce::ValueTree clipValueTree(SP_ID::CLIP);
          clipValueTree.setProperty(SP_ID::U_ID, dummyClip->getValueTree().getProperty(SP_ID::U_ID), nullptr);
          //set up ID
          clipValueTree.setProperty(SP_ID::clip_length_value,
                                    dummyClip->getValueTree().getProperty(SP_ID::clip_length_value),
                                    nullptr); //setup length
          auto newClip = new DummyClip(dummyClip->getWidth(), dummyClip->getHeight(), tile->getPosition(),
                                       clipValueTree, dummyClip->getClipColour());
          newClip->setUpValueTree(start, end, "dummyClip");
          track->trackAppendClipValueTree(clipValueTree);
          track->addAndMakeVisible(newClip);
        }
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
  explicit MainDeckGUI(const juce::ValueTree& v, FreeDeckGUI& fdeck)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195)), valueTree(v), freeDeck(fdeck)
  {
    setUpGrid(TRACK_WIDTH);
    //TRACK_WIDTH = getWidth(); //maybe?
  }

  void addTrack(juce::ValueTree& newNode)
  {
    auto* track = new MainDeckTrack(TRACK_WIDTH, TRACK_WIDTH, newNode,
                                    valueTree.getChildWithName(SP_ID::FREEDECK_BRANCH), freeDeck);
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

  FreeDeckGUI& freeDeck;
};




