//
// Created by Juan Diego on 12/30/24.
//
#pragma once
#include <DeckGUI.h>

/*TODO:
 *use Viewport to implement scrolling on these components
 *
 *
 */


class MixDeckTrack : public juce::Component,
                     public DeckGUI
{
public:

    MixDeckTrack(int width, int height): DeckGUI(width, height, juce::Colour::fromRGB(144,144,144))
    {
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    ~MixDeckTrack()
    {

    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BACKGROUND_COLOUR);
        g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
    }

    void resized() override
    {
    }

private:
};

class MixDeckGUI final : public juce::Component,
                         public DeckGUI
{
public:

    explicit MixDeckGUI(juce::ValueTree& valueTree)
        : DeckGUI(200, 200, juce::Colour::fromRGB(60,60,60))
    {
        setSize(WINDOW_WIDTH, WINDOW_WIDTH);
        addAndMakeVisible(resizableEdge);
        resizableEdge.setAlwaysOnTop(true);
        setUpGrid(TRACK_WIDTH);
    }

    ~MixDeckGUI()
    {

    }

    void addTrack()
    {
        addAndMakeVisible(mixDeckTracks.add(new MixDeckTrack (TRACK_WIDTH, TRACK_WIDTH)));
        grid.items.add(mixDeckTracks.getLast()); //always returns right component?
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

    void paint (juce::Graphics& g) override
    {
        g.fillAll(BACKGROUND_COLOUR);
        g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
        if (!mixDeckTracks.isEmpty())
            grid.performLayout (getLocalBounds());
    }

    void resized() override
    {
        const auto rect = getLocalBounds();
        resizableEdge.setBounds(rect.getX() + rect.getWidth() - RESIZABLE_EDGE, rect.getY(), RESIZABLE_EDGE, rect.getHeight());

        setUpGrid(rect.getWidth());
    }

private:
    juce::Grid grid;

    juce::ResizableEdgeComponent resizableEdge {this, nullptr, juce::ResizableEdgeComponent::Edge::rightEdge};

    juce::OwnedArray<MixDeckTrack> mixDeckTracks;

    int TRACK_WIDTH = 200;
    int TRACK_HEIGHT = 100;
};



