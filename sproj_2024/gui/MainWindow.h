//
// Created by Juan Diego on 10/4/24.
//
#pragma once
#include <MainComponent.h>

//==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (juce::String name, juce::ValueTree tree);

        void closeButtonPressed() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };