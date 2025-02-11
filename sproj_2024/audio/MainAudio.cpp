//
// Created by Juan Diego on 10/29/24.
//

#include "MainAudio.h"

MainAudio::MainAudio(juce::ValueTree v, SPCommandManager &manager, juce::AudioDeviceManager &audioManager)
    : audioGraph(new juce::AudioProcessorGraph()), valueTree(v), commandManager(manager), deviceManager(audioManager)
{
    audioGraph->enableAllBuses();

    deviceManager.initialiseWithDefaultDevices(1, 1); //1 inputs, 1 output
    deviceManager.addAudioCallback(audioPlayer.get());

    initGraph();

    audioPlayer->setProcessor(audioGraph.get());

    device.reset(deviceManager.getCurrentAudioDevice());

    baseSampleRate = deviceManager.getAudioDeviceSetup().sampleRate;

    commandManager.registerAllCommandsForTarget(this);
    commandManager.addTargetToCommandManager(this);
}

MainAudio::~MainAudio()
{
    deviceManager.removeAudioCallback(audioPlayer.get());
}

void MainAudio::initGraph()
{
    audioGraph->clear();

    inputNode = audioGraph->addNode(
        std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>
        (juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));

    outputNode = audioGraph->addNode(
        std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>
        (juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

    //initialize metronome
    metronome = audioGraph->addNode(std::make_unique<Clock>(valueTree));

    connectNode(metronome);

    addNewTrack();
}

void MainAudio::updateGraph()
{
}

void MainAudio::addNewTrack()
{
    const auto newTrack = audioGraph->addNode(std::make_unique<Track>(valueTree, commandManager));
    trackArray.add(newTrack);
    connectNode(newTrack);
}

void MainAudio::connectNode(const juce::AudioProcessorGraph::Node::Ptr &node) const
{
    for (int channel = 0; channel < 2; ++channel)
    {
        audioGraph->addConnection({
            {inputNode->nodeID, channel},
            {node->nodeID, channel}
        });
        audioGraph->addConnection({
            {node->nodeID, channel},
            {outputNode->nodeID, channel}
        });
    }
}

void MainAudio::pauseOrResumeProcessing() //There might be better ways of doing this?
{
    if (!isPlaying)
    {
        deviceManager.restartLastAudioDevice();
        isPlaying = true;
    }else
    {
        deviceManager.closeAudioDevice();
        isPlaying = false;
    }
}

//ApplicationCommandTarget methods

juce::ApplicationCommandTarget* MainAudio::getNextCommandTarget()
{
    return nullptr;
}

void MainAudio::getAllCommands(juce::Array<juce::CommandID> &c)
{
    juce::Array<juce::CommandID> commands{
        SP_CommandID::stopProcessing
    };
    c.addArray(commands);
}

void MainAudio::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID)
    {
        case SP_CommandID::stopProcessing:
            result.setInfo("Stop", "Stops playback", "Audio", 0);
            result.setTicked(false);
            result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers);
            break;
        default:
            break;
    }
}

bool MainAudio::perform(const InvocationInfo &info)
{
    switch (info.commandID)
    {
        case SP_CommandID::stopProcessing:
            pauseOrResumeProcessing();
            break;
        default:
            return false;
    }
    return true;
}




