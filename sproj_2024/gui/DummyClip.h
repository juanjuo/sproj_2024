//
// Created by Juan Diego on 2/2/25.
//

#pragma once
#include <DeckGUI.h>

/* BUGS!
 *
 * when dragging objects that are on top of the MixDeck the buffered image doesn't show correctly
 *
 */

class DummyClip final : public juce::Component,
                        public DeckGUI
{
public:
    DummyClip(const int width, const int height, juce::ValueTree& tree): DeckGUI(width, height, juce::Colour::fromRGB(144, 144, 144)), valueTree(tree)
    {
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setTopLeftPosition(calculateRandomPosition(getParentWidth(), getParentHeight()));
    }

    DummyClip(const int width, const int height, const int x, const int y, juce::ValueTree& tree): DeckGUI(width, height, juce::Colour::fromRGB(144, 144, 144)), valueTree(tree)
    {
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setTopLeftPosition(x, y);
    }

    // DummyClip(const DummyClip& clip, const juce::Point<int> point /*int x, int y*/): DeckGUI(clip.WINDOW_WIDTH, clip.WINDOW_HEIGHT, juce::Colour::fromRGB(144, 144, 144))
    // {
    //     setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //     setTopLeftPosition(point);
    //
    //     //probably set all of its member var//
    // }

    DummyClip(const juce::Point<int> point, juce::ValueTree& tree): DeckGUI(200, 100, juce::Colour::fromRGB(144, 144, 144)), valueTree(tree)
    {
        setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        setTopLeftPosition(point);
        //probably set all of its member var//
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

    void setFilePath(const juce::String& filePath)
    {

    }

    juce::ValueTree& getValueTree()
    {
        return valueTree;
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

};
