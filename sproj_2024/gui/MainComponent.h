#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <RulerDeckGUI.h>
#include <ControlDeckGUI.h>
#include <FreeDeckGUI.h>
#include <MixDeckGUI.h>
#include <MenuComponent.h>
#include <DeviceSelectionMenu.h>
#include <MainDeckHolder.h>

class MainComponent final : public juce::Component,
                            public juce::ApplicationCommandTarget
{
public:

    explicit MainComponent(const juce::ValueTree& tree, SPCommandManager& manager, juce::AudioDeviceManager& deviceManager);

    ~MainComponent() override;

    void createNewTrack();
    void createNewDummyClip();
    void initializeApplication();
    void startOrStopAnimation();

    //Component methods
    void paint (juce::Graphics&) override;
    void resized() override;
    void childBoundsChanged(Component *child) override;

    //ApplicationCommandTarget methods
    ApplicationCommandTarget *getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID> &c) override;
    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;
    bool perform(const InvocationInfo &info) override;

private:
    bool isPlaybackOn = false;

    SPCommandManager& commandManager;

    juce::AudioDeviceManager& deviceManager;

    std::shared_ptr<RulerDeckGUI> rulerDeckGUI;
    std::shared_ptr<ControlDeckGUI> controlDeckGui;
    std::shared_ptr<FreeDeckGUI> freeDeckGui;
    std::shared_ptr<MainDeckHolder> mainDeckHolder;
    std::shared_ptr<MixDeckGUI> mixDeckGui;

    // Menus
    std::shared_ptr<MenuComponent> menu;
    std::shared_ptr<DeviceSelectionMenu> deviceSelector;

    juce::ValueTree valueTree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
