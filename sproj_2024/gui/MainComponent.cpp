#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::ValueTree tree, SPCommandManager& manager, juce::AudioDeviceManager& deviceManager)
    : commandManager(manager), rulerDeckGUI(), controlDeckGui(tree), mainDeckGui(tree), freeDeckGui(tree),
      mixDeckGui(tree), menu(manager),
      deviceSelector(deviceManager, manager)

{
    if (tree.isValid()) std::cout << "is valid" << std::endl;
    setSize(600, 400);

    addAndMakeVisible(rulerDeckGUI);
    addAndMakeVisible(controlDeckGui);
    addAndMakeVisible(freeDeckGui);
    addAndMakeVisible(mixDeckGui);
    addAndMakeVisible(mainDeckGui);
    addAndMakeVisible(menu);
    addChildComponent(deviceSelector);

    commandManager.registerAllCommandsForTarget(this);
    commandManager.addTargetToCommandManager(this);
}

void MainComponent::createNewTrack()
{
    mixDeckGui.addTrack();
    mainDeckGui.addTrack();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    rulerDeckGUI.setBounds(getX(), getY(), getWidth(), controlDeckGui.getHeight());
    controlDeckGui.setBounds(getX(), getY(), mixDeckGui.getWidth(), controlDeckGui.getHeight());
    freeDeckGui.setBounds(getX(), getHeight() - freeDeckGui.getHeight(), getWidth(), freeDeckGui.getHeight());
    mixDeckGui.setBounds(getX(), getY() + controlDeckGui.getHeight(), mixDeckGui.getWidth(), getHeight()
                         - controlDeckGui.getHeight() - freeDeckGui.getHeight());
    mainDeckGui.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
                            - mixDeckGui.getWidth(), getHeight() - freeDeckGui.getHeight() - controlDeckGui.getHeight());
}

void MainComponent::childBoundsChanged(Component *child)
{
    controlDeckGui.setBounds(getX(), getY(), mixDeckGui.getWidth(), controlDeckGui.getHeight());
    freeDeckGui.setBounds(getX(), getHeight() - freeDeckGui.getHeight(), getWidth(), freeDeckGui.getHeight());
    mixDeckGui.setBounds(getX(), getY() + controlDeckGui.getHeight(), mixDeckGui.getWidth(), getHeight()
                         - controlDeckGui.getHeight() - freeDeckGui.getHeight());
    mainDeckGui.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
                            - mixDeckGui.getWidth(), getHeight() - freeDeckGui.getHeight() - controlDeckGui.getHeight());
}

//ApplicationCommandTarget methods

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return nullptr;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID> &c)
{
    juce::Array<juce::CommandID> commands{
        SP_CommandID::createNewTrack
    };
    c.addArray(commands);
}

void MainComponent::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID)
    {
    case SP_CommandID::createNewTrack:
        result.setInfo("create new track", "creates gui/audio for new track", "Audio", 0);
        result.setTicked(false);
        result.addDefaultKeypress('t', juce::ModifierKeys::commandModifier);
        break;
    default:
        break;
    }
}

bool MainComponent::perform(const InvocationInfo &info)
{
    switch (info.commandID)
    {
    case SP_CommandID::createNewTrack:
        createNewTrack();
        break;
    default:
        return false;
    }

    return true;
}

