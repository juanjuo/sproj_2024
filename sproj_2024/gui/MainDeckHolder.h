//
// Created by Juan Diego on 3/3/25.
//

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <MainDeckGUI.h>

class MainDeckHolder final : public juce::Component
{
public:
    explicit MainDeckHolder()
    {
        //setAlpha(0);
        setOpaque(false);
        setAlwaysOnTop(false);
    }

    void paint(juce::Graphics& g) override
    {
    }

    void resized() override
    {
        for (const auto deck : getChildren())
            deck->resized();
    }

    void startOrStopAnimation()
    {
        if (isMoving)
        {
            for (const auto component : getChildren())
            {
                MainDeckGUI* deck = dynamic_cast<MainDeckGUI*>(component);
                deck->stopAnimation();
            }
            isMoving = false;
        }
        else
        {
            for (const auto component : getChildren())
            {
                MainDeckGUI* deck = dynamic_cast<MainDeckGUI*>(component);
                deck->startAnimation();
            }
            isMoving = true;
        }
    }

private:
    bool isMoving = false;
};
