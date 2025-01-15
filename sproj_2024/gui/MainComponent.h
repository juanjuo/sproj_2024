

// CMake builds don't use an AppConfig.h, so it's safe to audio juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#audio <JuceHeader.h>` here instead, to make all your module headers visible.

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <ControlDeckGUI.h>
#include <FreeDeckGUI.h>
#include <MainDeckGUI.h>
#include <MixDeckGUI.h>
#include <MenuComponent.h>
#include <DeviceSelectionMenu.h>



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component
{
public:

    explicit MainComponent(juce::ValueTree tree, SPCommandManager& manager, juce::AudioDeviceManager& deviceManager);

    void paint (juce::Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component *child) override;

private:

    ControlDeckGUI controlDeckGui;
    MainDeckGUI mainDeckGui;
    FreeDeckGUI freeDeckGui;
    MixDeckGUI mixDeckGui;

    //Menus
    MenuComponent menu;
    DeviceSelectionMenu deviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
