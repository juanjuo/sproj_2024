//
// Created by Juan Diego on 3/3/25.
//

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <MainDeckGUI.h>
#include <MainDeckTiles.h>

class MainDeckMask final : public juce::Component,
                           public juce::DragAndDropTarget,
                           public juce::LassoSource<MainDeckTile*>,
                           public juce::ValueTree::Listener
{
public:
  explicit MainDeckMask(juce::ValueTree t, const juce::Array<MainDeckTile*>& tiles, FreeDeckGUI& fdeck) : trackValueTree(t), tileList(tiles), freeDeckGui(fdeck)
  {
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoFillColourId,
                             juce::Colours::blue);
    lassoComponent.setColour(juce::LassoComponent<MainDeckTile*>::ColourIds::lassoOutlineColourId,
                             juce::Colours::blueviolet);
    setAlwaysOnTop(true);
  }

  juce::ValueTree setUpClipValueTree(int start, int end, int length, int width, juce::Colour colour)
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
    //deckClipNode.addListener(this);
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

  void updateOccupied(bool state, int start, int length) const
  {
    int size = tileList.size();
    for (int i = 0; i < length; ++i)
    {
      int index = (start + i) % size; // Wrap around when reaching the end
      tileList[index]->setOccupied(state);
    }
  }

  bool areTilesOccupied(int start, int length) const
  {
    int size = tileList.size();
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

  void removeClip()
  {
  }

  void createNewClip(bool isBackwards)
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

  void addExistingClip(DummyClip* clip, MainDeckTile* tile)
  {
    const int start = tile->getTilePosition();
    int length = clip->getValueTree().getProperty(SP_ID::clip_length_value);
    auto end = start + length;

    if (areTilesOccupied(start, length))
    {
      juce::ValueTree clipValueTree(SP_ID::CLIP);
      //clipValueTree.addListener(this);
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

  // void updateOccupied(int start, int end, bool state) const
  // {
  //   for (int i = start; i < end; i++)
  //   {
  //     tileList[i]->setOccupied(state);
  //   }
  // }
  //
  // bool areTilesOccupied(int start, int end) const
  // {
  //   bool canPerform = true;
  //   for (int i = start; i < end; i++)
  //   {
  //     if (tileList[i]->getOccupied())
  //       canPerform = false;
  //   }
  //   return canPerform;
  // }

  void resized() override
  {
    //setBounds(trackValueTree->getBounds());
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
    // if (event.mods.isCtrlDown()) // delete not working
    // {
    //   auto tile = dynamic_cast<MainDeckTile*>(track->getComponentAt(event.position)); //better type checking here?
    //   if (tile != nullptr)
    //   {
    //     for (int i = 0; i < track->clips.size(); i++) // need to delete the element from the clip array afterward
    //     {
    //       DummyClip* clip = track->clips[i];
    //       if (clip->getBounds().contains(tile->getBounds()))
    //       {
    //         auto start = clip->getValueTree().getProperty(SP_ID::clip_start_value);
    //         auto end = clip->getValueTree().getProperty(SP_ID::clip_end_value);
    //         track->updateOccupied(start, end, false);
    //         track->getValueTree().removeChild((clip)->getValueTree(), nullptr); // delete tree
    //         track->removeChildComponent(clip);
    //         track->clips.remove(i);
    //       }
    //     }
    //   }
    //   lassoComponent.endLasso();
    // }
    // else
    // {
    auto mouseStart = event.getMouseDownPosition();
    auto mouseEnd = event.getPosition();
    bool isBackwards = mouseStart.x > mouseEnd.x;
    if (tilesSelected.getNumSelected() > 1)
      if (areTilesOccupied(tilesSelected.getSelectedItem(0)->getTilePosition(), tilesSelected.getNumSelected()))
        createNewClip(isBackwards);
    lassoComponent.endLasso();
    removeChildComponent(&lassoComponent);
    //}
  }

  //Drag and drop methods

  bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override
  {
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

  //Value Tree Listener methods
  // void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
  //                             const juce::Identifier& property) override
  // {
  //   if (property == SP_ID::clip_ready_to_play)
  //   {
  //     for (auto clip : trackValueTree)
  //     {
  //       if (clip.getProperty(SP_ID::U_ID) == treeWhosePropertyHasChanged.getProperty(SP_ID::U_ID))
  //       {
  //         clip.setProperty(SP_ID::clip_ready_to_play, treeWhosePropertyHasChanged.getProperty(SP_ID::clip_ready_to_play), nullptr);
  //       }
  //     }
  //     // auto freeDeckClip = freeDeckGui.getValueTree().getChildWithProperty(SP_ID::U_ID, treeWhosePropertyHasChanged.getProperty(SP_ID::U_ID));
  //     // freeDeckClip.setProperty(SP_ID::clip_ready_to_play, treeWhosePropertyHasChanged.getProperty(SP_ID::clip_ready_to_play), nullptr);
  //   }
  // }

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
    //setAlpha(0);
    setOpaque(false);
    setAlwaysOnTop(false);
    const auto deck1 = new MainDeckGUI(tree, fdeck, MainDeckGUI::MainDeckMode::on_Screen);
    const auto deck2 = new MainDeckGUI(tree, fdeck, MainDeckGUI::MainDeckMode::off_Screen);
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

    //grid.justifyContent = juce::Grid::JustifyContent::stretch; //technically not necessary?
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


