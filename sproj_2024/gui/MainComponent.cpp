#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(const juce::ValueTree& tree, SPCommandManager& manager, juce::AudioDeviceManager& dmanager)
    : commandManager(manager), deviceManager(dmanager), valueTree(tree)

{
    rulerDeckGUI = std::make_shared<RulerDeckGUI>();
    controlDeckGui = std::make_shared<ControlDeckGUI>(valueTree);
    freeDeckGui = std::make_shared<FreeDeckGUI>(valueTree);
    mainDeckHolder = std::make_shared<MainDeckHolder>(valueTree, *freeDeckGui);
    mixDeckGui = std::make_shared<MixDeckGUI>(valueTree);
    menu = std::make_shared<MenuComponent>(commandManager);
    deviceSelector = std::make_shared<DeviceSelectionMenu>(deviceManager, commandManager);

    setSize(600, 400);

    addAndMakeVisible(rulerDeckGUI.get());
    addAndMakeVisible(controlDeckGui.get());
    addAndMakeVisible(freeDeckGui.get());
    addAndMakeVisible(mixDeckGui.get());
    addAndMakeVisible(mainDeckHolder.get());
    addAndMakeVisible(menu.get());
    addChildComponent(deviceSelector.get());

    commandManager.registerAllCommandsForTarget(this);
    commandManager.addTargetToCommandManager(this);

    initializeApplication();
}

// MainComponent::~MainComponent()
// {
//     //deleteAllChildren();
// }

void MainComponent::createNewTrack()
{
    juce::ValueTree newNode(SP_ID::TRACK);
    SP::createNewID(newNode);
    auto trackBranch = valueTree.getChildWithName(SP_ID::TRACK_BRANCH);
    trackBranch.appendChild(newNode, nullptr);
    mixDeckGui->addTrack(newNode);
    mainDeckHolder->addTrack(newNode);
}

void MainComponent::createNewDummyClip() //not the fastest way of doing this (better to initialize this before hand)
{
    freeDeckGui->createNewDummyClip();
}

void MainComponent::initializeApplication() //only for the beta release of the application
{
    for (int i = 0; i < 6; i++)
    {
        createNewTrack();
    }
}

void MainComponent::startOrStopAnimation()
{
    mainDeckHolder->startOrStopAnimation();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    rulerDeckGUI->setBounds(getX(), getY(), getWidth(), controlDeckGui->getHeight());
    controlDeckGui->setBounds(getX(), getY(), mixDeckGui->getWidth(), controlDeckGui->getHeight());
    freeDeckGui->setBounds(getX(), getHeight() - freeDeckGui->getHeight(), getWidth(), freeDeckGui->getHeight());
    mixDeckGui->setBounds(getX(), getY() + controlDeckGui->getHeight(), mixDeckGui->getWidth(), getHeight()
                         - controlDeckGui->getHeight() - freeDeckGui->getHeight());
    mainDeckHolder->setBounds(getX() + mixDeckGui->getWidth(), getY() + controlDeckGui->getHeight(), getWidth()
                             - mixDeckGui->getWidth(),
                             getHeight() - freeDeckGui->getHeight() - controlDeckGui->getHeight());
    mainDeckHolder->resized();
}

void MainComponent::childBoundsChanged(Component* child)
{
    juce::ignoreUnused(child);

    controlDeckGui->setBounds(getX(), getY(), mixDeckGui->getWidth(), controlDeckGui->getHeight());

    freeDeckGui->setBounds(getX(), getHeight() - freeDeckGui->getHeight(), getWidth(), freeDeckGui->getHeight());

    mixDeckGui->setBounds(getX(), getY() + controlDeckGui->getHeight(), mixDeckGui->getWidth(),
                          getHeight() - controlDeckGui->getHeight() - freeDeckGui->getHeight());

    mainDeckHolder->setBounds(getX() + mixDeckGui->getWidth(), getY() + controlDeckGui->getHeight(),
                              getWidth() - mixDeckGui->getWidth(),
                              getHeight() - freeDeckGui->getHeight() - controlDeckGui->getHeight());
}

//ApplicationCommandTarget methods

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
    return nullptr;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID>& c)
{
    juce::Array<juce::CommandID> commands{
        SP_CommandID::createNewTrack,
        SP_CommandID::createNewDummyClip
    };
    c.addArray(commands);
}

void MainComponent::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case SP_CommandID::createNewTrack:
        result.setInfo("create new track", "creates gui/audio for new track", "Audio", 0);
        result.setTicked(false);
        result.addDefaultKeypress('t', juce::ModifierKeys::commandModifier);
        break;
    case SP_CommandID::createNewDummyClip:
        result.setInfo("create new Dummy Clip", "creates new dummy clip", "Audio", 0);
        result.setTicked(false);
        result.addDefaultKeypress('d', juce::ModifierKeys::commandModifier);
        break;
    default:
        break;
    }
}

bool MainComponent::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
    case SP_CommandID::createNewTrack:
        createNewTrack();
        break;
    case SP_CommandID::createNewDummyClip:
        //createNewDummyClip();
        SP::printVT(valueTree);
        break;
    default:
        return false;
    }

    return true;
}

