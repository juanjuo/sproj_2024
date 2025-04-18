//
// Created by Juan Diego on 2/2/25.
//

#pragma once
#include <DeckGUI.h>
#include "helpers.h"


class DummyClip final : public juce::Component,
                        public DeckGUI
{
public:

    //for creating clips for FreeDeck
    DummyClip(const int width, const int height, const juce::Rectangle<int> parentBounds, const juce::ValueTree& tree, const juce::Colour colour): DeckGUI(width, height, colour), valueTree(tree)
    {
        setSize(width, height);
        setTopLeftPosition(calculateRandomPosition(parentBounds.getWidth(), parentBounds.getHeight()));
        initializeValueTree();
    }

    // for creating clips on MainDeck
    DummyClip(const int width, const int height, const juce::Point<int> point, const juce::ValueTree& tree, const juce::Colour colour): DeckGUI(width, height, colour), valueTree(tree)
    {
        setSize(width, height);
        setTopLeftPosition(point);
        initializeValueTree();
        //probably set all of its member var//
    }

    juce::Colour getClipColour() const
    {
        return BACKGROUND_COLOUR;
    }


    static juce::Point<int> calculateRandomPosition(const int width, const int height) //maybe a better way of doing this?
    {
        const int minX = static_cast<int> (width * 0.05);
        const int maxX = static_cast<int> (width * 0.85);
        const int minY = static_cast<int> (height * 0.01);
        const int maxY = static_cast<int> (height * 0.15);

        int x = minX + rand() % (maxX - minX + 1);
        int y = minY + rand() % (maxY - minY + 1);

        return {x, y};
    }

    void setUpValueTree(const int startingValue = -1, const int endValue = -1, const int length = -1, const juce::String& filePath = " ")
    {
        valueTree.setProperty(SP_ID::clip_start_value, startingValue, nullptr);
        valueTree.setProperty(SP_ID::clip_end_value, endValue, nullptr);
        valueTree.setProperty(SP_ID::clip_filepath, filePath, nullptr);
        valueTree.setProperty(SP_ID::clip_length_value, length, nullptr);
    }

    void initializeValueTree()
    {
        valueTree.setProperty(SP_ID::clip_start_value, -1, nullptr);
        valueTree.setProperty(SP_ID::clip_end_value, -1, nullptr);
        valueTree.setProperty(SP_ID::clip_filepath, localFilePath, nullptr);
    }

    juce::ValueTree& getValueTree()
    {
        return valueTree;
    }

    juce::String getFilePath()
    {
        return localFilePath;
    }

    void mouseDrag(const juce::MouseEvent& event) override //maybe call startDragging directly?
    {
        juce::String description{"dragging clip"};
        startDragAndDrop(event, description, true);
        isBeingDragged = true;
        //std::cout << "IS DRAGGING" << std::endl;
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        juce::ignoreUnused(event);
        isBeingDragged = false;
        //std::cout << "STOP DRAGGING" << std::endl;
    }


    void paint(juce::Graphics& g) override
    {
        g.setColour(BACKGROUND_COLOUR);
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds(), BORDER_WIDTH - 1);
    }

    void resized() override
    {
    }


    void startDragAndDrop(const juce::MouseEvent& e, const juce::var& dragDescription, bool allowDraggingToOtherWindows)
    {
        if (auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this))
        {
            int x = 0;
            int y = 0;
            auto dragImage = createComponentSnapshot(getBounds(), true);

            auto p = juce::Point<int>(x, y) - e.getEventRelativeTo(this).position.toInt();

            // ReSharper disable once CppDeprecatedEntity
            dragContainer->startDragging(dragDescription, this, dragImage, allowDraggingToOtherWindows, &p, &e.source);
        }
        else
        {
            // to be able to do a drag-and-drop operation, this component needs to
            // be inside a component which is also a DragAndDropContainer.
            jassertfalse;
        }
    }

private:
    bool isBeingDragged = false;

    juce::ValueTree valueTree;


    juce::String localFilePath {" "};

    static int calculateLengthInBeats(const int start, const int end)
    {
        return end - start; // + 1 since its 0 indexed
    }

    static int calculateEndPosition(const int start, const int length)
    {
        return start + length ;
    }
};
