//
// Created by Juan Diego on 3/16/25.
//
#pragma once
#include <MainDeckGUI.h>

class MainDeckTile final : public juce::Component
{
public:
    MainDeckTile(int width, int height, const int p) : position(p)
    {
        setAlwaysOnTop(true);
        setSize(width, height);
    }

    void reset()
    {
        selectedColour = juce::Colour::fromRGBA(0, 0, 0, 0);
        valueTree = juce::ValueTree();
        isOccupied = false;
    }

    void setClip(const juce::ValueTree tree, const juce::Colour colour)
    {
        selectedColour = colour;
        valueTree = tree;
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

    juce::Rectangle<int> getTileBounds()
    {
        return getLocalArea(getParentComponent(), getBoundsInParent());
    }

private:
    int position; //represents how many beats until playback starts. (maybe this number must be in ValueTree

    bool isOccupied = false; //Active = can be assigned a clip

    juce::ValueTree valueTree;

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
};
