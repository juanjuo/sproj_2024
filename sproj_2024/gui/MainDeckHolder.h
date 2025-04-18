//
// Created by Juan Diego on 3/3/25.
//

#pragma once
#include <MainDeckGUI.h>
#include <MainDeckTiles.h>
#include <focusrite/e2e/ComponentSearch.h>

class MainDeckMask final : public juce::Component,
                           public juce::DragAndDropTarget,
                           public juce::LassoSource<MainDeckTile*>,
                           public juce::ValueTree::Listener
{
public:
  explicit MainDeckMask(const juce::ValueTree& t, const juce::Array<MainDeckTile*>& tiles, FreeDeckGUI& fdeck) : trackValueTree(t), freeDeckGui(fdeck), tileList(tiles)
  {
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoFillColourId,
                             juce::Colours::blue);
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoOutlineColourId,
                             juce::Colours::blueviolet);
    setAlwaysOnTop(true);
  }

  juce::ValueTree setUpClipValueTree(const int start, const int end, const int length, const int width, const juce::Colour colour)
  {
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    juce::ValueTree deckClipNode(SP_ID::CLIP); //add ValueTree to freedeck
    SP::createNewID(trackClipNode);
    deckClipNode.setProperty(SP_ID::U_ID, trackClipNode.getProperty(SP_ID::U_ID), nullptr);

    //set up value tree for FREEDECK
    freeDeckGui.addAndMakeVisible(new DummyClip(width, 100, freeDeckGui.getLocalBounds(), deckClipNode,
                                                colour)); //add component to freeDeck
    deckClipNode.setProperty(SP_ID::clip_length_value, length, nullptr); //set up length
    deckClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    freeDeckGui.getValueTree().appendChild(deckClipNode, nullptr); //add at the end so all values are updated

    //set up value tree for TRACK
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, " ", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    //trackClipNode.addListener(this);
    trackValueTree.appendChild(trackClipNode, nullptr);

    return trackClipNode;
  }

  void updateOccupied(const bool state, const int start, const int length) const
  {
    int size = tileList.size();
    for (int i = 0; i < length; ++i)
    {
      const int index = (start + i) % size; // Wrap around when reaching the end
      tileList[index]->setOccupied(state);
    }
  }

  bool areTilesOccupied(const int start, const int length) const
  {
    const int size = tileList.size();
    for (int i = 0; i < length; ++i)
    {
      int index = (start + i) % size; // Wrap around when reaching the end
      if (tileList[index]->getOccupied())
      {
        return false; // Exit early if any tile is occupied
      }
    }
    return true;
  }

  void createNewClip(const bool isBackwards)
  {
    const auto colour = juce::Colour::fromRGB(rand() % 255, rand() % 255, rand() % 255);
    const auto size = tilesSelected.getNumSelected();
    auto start = 0;
    auto end = 0;
    if (isBackwards)
    {
      start = tilesSelected.getSelectedItem(size - 1)->getTilePosition();
      end = tilesSelected.getSelectedItem(0)->getTilePosition();
    }
    else
    {
      start = tilesSelected.getSelectedItem(0)->getTilePosition();
      end = tilesSelected.getSelectedItem(size - 1)->getTilePosition();
    }
    int totalWidth = 0;
    for (const auto tile : tilesSelected)
    {
      totalWidth = totalWidth + tile->getWidth();
      //std::cout << tile->getTilePosition() << std::endl;
    }
    const juce::ValueTree tree = setUpClipValueTree(start, end + 1, size, totalWidth, colour);
    for (const auto tile : tilesSelected)
    {
      tile->setClip(tree, colour);
      tile->setOccupied(true);
    }
  }

  void addExistingClip(DummyClip* clip, const MainDeckTile* tile)
  {
    const int start = tile->getTilePosition();
    int length = clip->getValueTree().getProperty(SP_ID::clip_length_value);
    auto end = start + length;

    if (areTilesOccupied(start, length))
    {
      juce::ValueTree clipValueTree(SP_ID::CLIP);
      clipValueTree.setProperty(SP_ID::U_ID, clip->getValueTree().getProperty(SP_ID::U_ID), nullptr);
      clipValueTree.setProperty(SP_ID::clip_filepath, clip->getValueTree().getProperty(SP_ID::clip_filepath), nullptr);
      clipValueTree.setProperty(SP_ID::clip_end_value, end, nullptr);
      clipValueTree.setProperty(SP_ID::clip_length_value, length, nullptr);
      clipValueTree.setProperty(SP_ID::clip_start_value, start, nullptr);
      clipValueTree.setProperty(SP_ID::clip_ready_to_play, clip->getValueTree().getProperty(SP_ID::clip_ready_to_play), nullptr);
      auto colour = clip->getClipColour();
      int size = tileList.size();
      for (int i = 0; i < length; ++i)
      {
        int index = (start + i) % size;
        tileList[index]->setOccupied(true);
        tileList[index]->setClip(clipValueTree, colour);
      }
      trackValueTree.appendChild(clipValueTree, nullptr);
    }
  }

  void resized() override
  {
  }

  void mouseDown(const juce::MouseEvent& event) override
  {
    addAndMakeVisible(lassoComponent);
    lassoComponent.beginLasso(event, this);
  }

  void mouseDrag(const juce::MouseEvent& event) override
  {
    lassoComponent.dragLasso(event);
  }

  void mouseUp(const juce::MouseEvent& event) override
  {
    auto mouseStart = event.getMouseDownPosition();
    auto mouseEnd = event.getPosition();
    bool isBackwards = mouseStart.x > mouseEnd.x;
    if (tilesSelected.getNumSelected() > 1)
      if (areTilesOccupied(tilesSelected.getSelectedItem(0)->getTilePosition(), tilesSelected.getNumSelected()))
        createNewClip(isBackwards);
    lassoComponent.endLasso();
    removeChildComponent(&lassoComponent);
  }

  //Drag and drop methods
  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
    juce::ignoreUnused(dragSourceDetails);
    return true;
  }

  void itemDropped(const SourceDetails& dragSourceDetails) override //is there better ways of doing this??
  {
    auto* dummyClip = dynamic_cast<DummyClip*>(dragSourceDetails.sourceComponent.get());
    auto point = dragSourceDetails.localPosition;
    if (dummyClip != nullptr)
    {
      for (auto tile : tileList)
      {
        if (tile->getScreenBounds().contains(localPointToGlobal(point)))
        {
          addExistingClip(dummyClip, tile);
          break;
        }
      }
    }
  }

  // LassoSource methods
  void findLassoItemsInArea(juce::Array<MainDeckTile*>& itemsFound, const juce::Rectangle<int>& area) override
  {
    juce::ignoreUnused(area);
    for (auto element : tileList)
    {
      if (lassoComponent.getScreenBounds().intersects(element->getScreenBounds()))
      {
        itemsFound.add(element);
      }
    }
  }

  juce::SelectedItemSet<MainDeckTile*>& getLassoSelection() override
  {
    return tilesSelected;
  }

private:
  juce::ValueTree trackValueTree;

  FreeDeckGUI& freeDeckGui;

  SPSelectedItemSet tilesSelected; //list for all tiles selected by lasso

  juce::Array<MainDeckTile*> tileList;

  juce::LassoComponent<MainDeckTile*> lassoComponent;
};

class MainDeckHolder final : public juce::Component
{
public:
  explicit MainDeckHolder(const juce::ValueTree& tree, FreeDeckGUI& fdeck) : freeDeckGui(fdeck)
  {
    setOpaque(false);
    setAlwaysOnTop(false);
    const auto deck1 = new MainDeckGUI(tree, MainDeckGUI::MainDeckMode::on_Screen);
    const auto deck2 = new MainDeckGUI(tree, MainDeckGUI::MainDeckMode::off_Screen);
    mainDecks.add(deck1);
    mainDecks.add(deck2);
    addAndMakeVisible(deck1); //on-screen
    addAndMakeVisible(deck2); //off-screen
    setUpGrid(getWidth());
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

    grid.autoFlow = juce::Grid::AutoFlow::row;
  }

  juce::Array<MainDeckTile*> createTiles(const int numOfTiles)
  {
    juce::Array<MainDeckTile*> tiles;

    for (int i = 0; i < numOfTiles; i++)
    {
      auto* tile = new MainDeckTile(TILE_WIDTH, TILE_HEIGHT, i);
      tiles.add(tile);
    }

    return tiles;
  }

  void addTrack(juce::ValueTree tree) //add MASK to holder and track to MainDeck
  {
    auto tiles = createTiles(NUM_TILES);
    juce::Array<MainDeckTile*> firstHalf;
    juce::Array<MainDeckTile*> secondHalf;
    for (int i = 0; i < NUM_TILES; i++)
    {
      if (i < NUM_TILES / 2)
      {
        firstHalf.add(tiles[i]);
      }
      else
      {
        secondHalf.add(tiles[i]);
      }
    }
    mainDecks[0]->addTrack(tree, firstHalf);
    mainDecks[1]->addTrack(tree, secondHalf);
    auto mask = new MainDeckMask(tree, tiles, freeDeckGui);
    focusrite::e2e::ComponentSearch::setTestId (*mask, "test_mask"); //for testing
    mask->setSize(getWidth(), TRACK_HEIGHT);
    grid.items.add(mask); //always returns right component?
    addAndMakeVisible(mask);
  }

  void startOrStopAnimation()
  {
    if (isMoving)
    {
      for (const auto deck : mainDecks)
      {
        deck->stopAnimation();
      }
      isMoving = false;
    }
    else
    {
      for (const auto deck : mainDecks)
      {
        deck->startAnimation();
      }
      isMoving = true;
    }
  }

  void paint(juce::Graphics& g) override
  {
    juce::ignoreUnused(g);
    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    for (const auto deck : getChildren())
      deck->resized();
    setUpGrid(getWidth());
  }

private:
  juce::Array<MainDeckGUI*> mainDecks;

  juce::Grid grid;

  FreeDeckGUI& freeDeckGui;

  int TRACK_WIDTH = 1500;
  int TRACK_HEIGHT = 100;

  int TILE_WIDTH = 15;
  int TILE_HEIGHT = 100;
  const int NUM_TILES = 80 * 2;

  bool isMoving = false;
};


