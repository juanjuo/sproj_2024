//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <DeckGUI.h>
#include <DummyClip.h>
#include <helpers.h>
#include <juce_animation/juce_animation.h>
#include <MainDeckTiles.h>

//this is responsible for showing and organizing a grid of draggable components, with the height of a track
class MainDeckTrack final : public juce::Component,
                            public DeckGUI,
                            public juce::DragAndDropContainer
  //public juce::ChangeListener
{
public:
  //represents every individual tile in the grid where audio files can be positioned
  MainDeckTrack(int width, int height, juce::ValueTree& n, juce::ValueTree freeDeck, FreeDeckGUI& fdeck,
                const juce::Array<MainDeckTile*>& tiles)
    : DeckGUI(width, height, juce::Colour::fromRGB(195, 195, 195)), node(n), tileList(tiles)
  {
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setUpGrid();
    setPaintingIsUnclipped(true);
    addTiles();
  }

  juce::ValueTree getValueTree()
  {
    return node;
  }

  void addTiles()
  {
    for (auto tile : tileList)
    {
      addAndMakeVisible(tile);
      grid.items.add(tile);
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

  void resetTrack() //deletes all clips held by this track
  {
    for (auto tile : tileList)
    {
      if (tile->getOccupied())
      {
        tile->reset();
      }
    }
    //node.removeAllChildren(nullptr);
  }

  void paint(juce::Graphics& g) override
  {
    g.setColour(BACKGROUND_COLOUR);
    g.fillRect(getLocalBounds());
    //g.fillAll(juce::Colour::fromRGB(60, 60, 60));
    //g.drawRect(this->getLocalBounds(), BORDER_WIDTH - 1);
    for (int i = 0; i < getWidth(); i = (i + ((getWidth() / NUM_BARS) * 2)))
    //TILE coloring! does not work dynamically
    {
      g.setColour(juce::Colour::fromRGB(175, 175, 175));
      g.fillRect(i, 0, getWidth() / NUM_BARS, getHeight());
    }

    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    //setUpGrid();
  }

private
:
  juce::Grid grid;

  juce::ValueTree node; //this track's value tree
  juce::Array<MainDeckTile*> tileList;

  int TILE_WIDTH = 15;
  int TILE_HEIGHT = 100;

  const int NUM_TILES = 80;

  const int NUM_BARS = 20;
};

class MainDeckGUI final : public juce::Component,
                          public DeckGUI,
                          public juce::ValueTree::Listener
{
public:
  enum class MainDeckMode
  {
    on_Screen = 0,
    off_Screen
  };

  explicit MainDeckGUI(const juce::ValueTree& v, FreeDeckGUI& fdeck, MainDeckMode mode)
    : DeckGUI(0, 0, juce::Colour::fromRGB(195, 195, 195)), valueTree(v), freeDeck(fdeck)
  {
    deck_mode = mode;
    setAlwaysOnTop(true);
    if (deck_mode == MainDeckMode::on_Screen)
    {
      setTopLeftPosition(0, 0);
      currentAnimationPosition = 0.5;
    }
    if (deck_mode == MainDeckMode::off_Screen)
    {
      setTopLeftPosition(getParentWidth(), 0);
      currentAnimationPosition = 0.0;
    }

    setUpGrid(TRACK_WIDTH);

    setPaintingIsUnclipped(true);
    //TRACK_WIDTH = getWidth(); //maybe?
  }

  void addTrack(juce::ValueTree& newNode, const juce::Array<MainDeckTile*>& tiles)
  {
    newNode.addListener(this);
    auto* track = new MainDeckTrack(TRACK_WIDTH, TRACK_HEIGHT, newNode,
                                    valueTree.getChildWithName(SP_ID::FREEDECK_BRANCH), freeDeck, tiles);
    grid.items.add(track);
    addAndMakeVisible(track);
  }

  // void childBoundsChanged(Component* child) override //to update mask bounds! BUG
  // {
  //   for (auto mask : masks)
  //     mask->resized();
  // }

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

    //update masks position
    // for (auto mask : masks)
    //   mask->resized();
  }

  void resetAllTracks() //for when animation re starts
  {
    for (auto component : getChildren())
    {
      auto track = dynamic_cast<MainDeckTrack*>(component);
      track->resetTrack();
    }
  }

  void paint(juce::Graphics& g) override
  {
    g.setColour(BACKGROUND_COLOUR);
    g.fillRect(getLocalBounds());
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), BORDER_WIDTH);
    if (getNumChildComponents() != 0)
      grid.performLayout(getLocalBounds());
  }

  void resized() override
  {
    setBounds(getParentComponent()->getBounds());
    if (deck_mode == MainDeckMode::on_Screen)
      setTopLeftPosition(0, 0);
    if (deck_mode == MainDeckMode::off_Screen)
      setTopLeftPosition(getParentWidth(), 0);
    setUpGrid(getWidth());
  }

  void startAnimation()
  {
    if (lastAnimation != nullptr) //these objects are lightweight references to the same underlying instance
    {
      updater.removeAnimator(*lastAnimation); //no need to get deleted manually maybe?
    }
    lastAnimation = new juce::Animator(generateAnimation(calculateSpeed()));
    //updater.addAnimator(animator, [this] { updater.removeAnimator(animator); });
    updater.addAnimator(*lastAnimation);
    lastAnimation->start();
  }

  void stopAnimation() const
  {
    lastAnimation->complete();
  }

private:
  MainDeckMode deck_mode;

  juce::ValueTree valueTree;

  //bool isEmpty = true; //checks whether this Main Deck has any track with clips

  float currentAnimationPosition;

  juce::Animator* lastAnimation;

  int calculateSpeed()
  {
    int bpm = valueTree.getChildWithName(SP_ID::METRONOME_BRANCH).getProperty(SP_ID::metronome_bpm);
    if (bpm <= 0)
    {
      //std::cerr << "BPM must be a positive integer." << std::endl;
      return -1; // Indicate an error
    }
    int finalMs = static_cast<int>(60000.0f / bpm) * 160;
    std::cout << finalMs << std::endl;
    return static_cast<int>(60000.0f / bpm) * 160; //time in ms for every beat, multiplied by the number of beats * 2
  }

  juce::Animator generateAnimation(const int speed)
  {
    juce::Animator animator = [&]
    {
      return juce::ValueAnimatorBuilder{}
             .runningInfinitely()
             .withEasing(juce::Easings::createLinear())
             .withOnStartReturningValueChangedCallback(
               [this]
               {
                 const auto width = getParentWidth();
                 const auto limits = juce::makeAnimationLimits(width, -width);

                 const float newAnimationPosition = currentAnimationPosition;

                 return [this, limits, newAnimationPosition, width](auto value)
                 {
                   auto position = std::fmod(value + newAnimationPosition, 1.0f);
                   setTopLeftPosition(limits.lerp(position), 0); //start in the middle
                   if (value != 1)
                   //can lead to bugs! if it happens to land right on 1, it will reset the whole screen
                   {
                     currentAnimationPosition = position;
                   }
                   if (getX() <= width && getX() >= width - 10) //bounds checking for resetting the screen
                   {
                     resetAllTracks();
                   }
                 };
               })
             .withOnCompleteCallback([this]
             {
               const auto width = getParentWidth();
               const auto limits = juce::makeAnimationLimits(width, -width);
               setTopLeftPosition(limits.lerp(currentAnimationPosition), 0);
             })
             .withDurationMs(speed)
             .build();
    }();

    return animator;
  }

  juce::VBlankAnimatorUpdater updater{this};

  juce::Grid grid;

  int TRACK_WIDTH = 1500;
  int TRACK_HEIGHT = 100;

  FreeDeckGUI& freeDeck;
};




