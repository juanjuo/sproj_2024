#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::ValueTree& tree, SPCommandManager& manager, juce::AudioDeviceManager& deviceManager)
    : commandManager(manager), rulerDeckGUI(), controlDeckGui(tree), freeDeckGui(tree),
      mixDeckGui(tree), menu(manager),
      deviceSelector(deviceManager, manager), valueTree(tree), mainDeckHolder(tree, freeDeckGui)

{

    if (tree.isValid()) std::cout << "is valid" << std::endl;
    setSize(600, 400);

    addAndMakeVisible(rulerDeckGUI);
    addAndMakeVisible(controlDeckGui);
    addAndMakeVisible(freeDeckGui);
    addAndMakeVisible(mixDeckGui);
    addAndMakeVisible(mainDeckHolder);
    addAndMakeVisible(menu);
    addChildComponent(deviceSelector);

    commandManager.registerAllCommandsForTarget(this);
    commandManager.addTargetToCommandManager(this);

    initializeApplication();
}

void MainComponent::createNewTrack()
{
    juce::ValueTree newNode (SP_ID::TRACK);
    SP::createNewID(newNode);
    auto trackBranch = valueTree.getChildWithName(SP_ID::TRACK_BRANCH);
    trackBranch.appendChild(newNode, nullptr);
    mixDeckGui.addTrack(newNode);
    mainDeckHolder.addTrack(newNode);
}

void MainComponent::createNewDummyClip() //not the fastest way of doing this (better to initialize this before hand)
{
    freeDeckGui.createNewDummyClip();
}

void MainComponent::initializeApplication() //only for the beta release of the application
{
    for (int i = 0; i < 1; i++)
    {
        createNewTrack();
    }
}

void MainComponent::startOrStopAnimation()
{
    mainDeckHolder.startOrStopAnimation();
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
    mainDeckHolder.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
                            - mixDeckGui.getWidth(), getHeight() - freeDeckGui.getHeight() - controlDeckGui.getHeight());
    mainDeckHolder.resized();
}

void MainComponent::childBoundsChanged(Component *child)
{
    controlDeckGui.setBounds(getX(), getY(), mixDeckGui.getWidth(), controlDeckGui.getHeight());
    freeDeckGui.setBounds(getX(), getHeight() - freeDeckGui.getHeight(), getWidth(), freeDeckGui.getHeight());
    mixDeckGui.setBounds(getX(), getY() + controlDeckGui.getHeight(), mixDeckGui.getWidth(), getHeight()
                         - controlDeckGui.getHeight() - freeDeckGui.getHeight());
    mainDeckHolder.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
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
        SP_CommandID::createNewTrack,
        SP_CommandID::createNewDummyClip
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
    case SP_CommandID::createNewDummyClip:
        result.setInfo("create new Dummy Clip", "creates new dummy clip", "Audio", 0);
        result.setTicked(false);
        result.addDefaultKeypress('d', juce::ModifierKeys::commandModifier);
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
    case SP_CommandID::createNewDummyClip:
        //createNewDummyClip();
        SP::printVT(valueTree);
        break;
    default:
        return false;
    }

    return true;
}

/*
 * APPLICATION ANIMATION SKETCH
 *
 * MainComponent CLASS
 *
 *
 *
 *
 *
 * CommandTarget:
 * - on spacebar:
 * - - if !isMoving
 * - - - updater.addAnimator(MainDeckGUI.getAnimator (1), [this] { updater.removeAnimator (MainDeckGUI.getAnimator);) //start moving
 * - - - updater.addAnimator(MainDeckGUI.getAnimator (2), [this] { updater.removeAnimator (MainDeckGUI.getAnimator);)
 * - - - MainDeckGUI.getAnimator (1).start()
 * - - - MainDeckGUI.getAnimator (2).start()
 * - - else
 * - - - MainDeckGUI.getAnimator (1).complete() //Stop moving
 * - - - MainDeckGUI.getAnimator (1).complete()
 *
 *  //How can I have an infinite animation that goes from 0 to 1? (instead of just going 0 to infinity)
 *  // I can use std::fmod from the <cmath> library
 *
 *
 * members:
 * bool isMoving
 * VBlankAnimatorUpdater updater
 * MainDeckHolder //the space that the MainDeckGUI's will use
 *
 * MainDeckGUI CLASS
 *
* Animator animator = ValueAnimatorBuilder {}.withEasing (Easings::createLinear())
*                                            .runningInfinitely()
                                             .withOnStartReturningValueChangedCallback (
                                           [this]
                                           {
                                               const auto width = getParentWidth() - 2 * margin;
                                               const auto height = 130;
                                               setBounds (-width, margin, width, height);
                                               setVisible (true);

                                               const auto limits = makeAnimationLimits (-width, width*2);

                                               return [this, limits] (auto value)
                                               {
                                                   const auto progress = std::clamp (shouldOpen ? value : 1.0 - value, 0.0, 1.0);
                                                   setTopLeftPosition (roundToInt (limits.lerp ((float) progress)), margin);
                                               };
                                           })
                                            .withOnCompleteCallback ( [this] {

                                            })
                                            .build();
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

