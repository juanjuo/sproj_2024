#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to audio juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#audio <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include <ClockGUI.h>
#include <Clock.h>
#include <ClockController.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final : public juce::Component
{
public:

    explicit MainComponent(juce::ValueTree tree);

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    ClockGUI clockGui;
    //juce::ValueTree mainValueTree {mainValueTreeName};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
