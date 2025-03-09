//
// Created by Juan Diego on 2/2/25.
//

#pragma once
#include <DeckGUI.h>
#include "Identifiers.h"
#include "FreeDeckGUI.h"

/* BUGS!
 *
 * when dragging objects that are on top of the MixDeck the buffered image doesn't show correctly
 *
 */

class DummyClip final : public juce::Component,
                        public DeckGUI
{
public:

    //for creating clips for FreeDeck
    DummyClip(const int width, const int height, const juce::Rectangle<int> parentBounds, juce::ValueTree& tree, juce::Colour colour): DeckGUI(width, height, colour), valueTree(tree)
    {
        setSize(width, height);
        setTopLeftPosition(calculateRandomPosition(parentBounds.getWidth(), parentBounds.getHeight()));
        initializeValueTree();
    }

    // for creating clips on MainDeck
    DummyClip(const int width, const int height, const juce::Point<int> point, juce::ValueTree& tree, juce::Colour colour): DeckGUI(width, height, colour), valueTree(tree)
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


    juce::Point<int> calculateRandomPosition(const int width, const int height) //maybe a better way of doing this?
    {
        int minX = width * 0.05;
        int maxX = width * 0.85;
        int minY = height * 0.01;
        int maxY = height * 0.15;

        int x = minX + rand() % (maxX - minX + 1);
        int y = minY + rand() % (maxY - minY + 1);

        return {x, y};
    }

    void setUpValueTree(const int startingValue = -1, const int endValue = -1, const juce::String& filePath = "")
    {
        valueTree.setProperty(SP_ID::clip_start_value, startingValue, nullptr);
        valueTree.setProperty(SP_ID::clip_end_value, endValue, nullptr);
        valueTree.setProperty(SP_ID::clip_filepath, localFilePath, nullptr);
        valueTree.setProperty(SP_ID::clip_length_value, calculateLengthInBeats(startingValue, endValue), nullptr);
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
        isBeingDragged = false;
        //std::cout << "STOP DRAGGING" << std::endl;
    }


    void paint(juce::Graphics& g) override
    {
        g.fillAll(BACKGROUND_COLOUR);
        g.drawRect(this->getLocalBounds(), BORDER_WIDTH - 1);
        //setTopLeftPosition();
    }

    void resized() override
    {
    }


    void startDragAndDrop(const juce::MouseEvent& e, const juce::var& dragDescription, bool allowDraggingToOtherWindows)
    {
        if (auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this))
        {
            int x, y;
            auto dragImage = createComponentSnapshot(getBounds(), true);

            auto p = juce::Point<int>(x, y) - e.getEventRelativeTo(this).position.toInt();
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
    bool isBeingDragged = false; //if I ever want to make the dragAndDrop look better

    juce::ValueTree valueTree;


    //juce::String localFilePath {" "};

    juce::String localFilePath {"/Users/juan/Desktop/Sunny2.wav"};

    //juce::File filePath {juce::File("/Users/juan/Desktop/Sunny2.wav")};

    int calculateLengthInBeats(const int start, const int end)
    {
        return end - start;
    }

    int calculateEndPosition(const int start, int length)
    {
        return start + length;
    }
};
