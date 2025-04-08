//
// Created by Juan Diego on 10/4/24.
//
#pragma once
#include <MainComponent.h>
//#include "melatonin_inspector/melatonin_inspector.h"


//==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow final : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (const juce::String& name, MainComponent* mainComponent);

        void closeButtonPressed() override;

    private:
        //melatonin::Inspector inspector { *this }; //MELATONIN WINDOW

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };