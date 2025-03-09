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


    audioPlayer->setProcessor(audioGraph.get());

    device.reset(deviceManager.getCurrentAudioDevice());

    baseSampleRate = deviceManager.getAudioDeviceSetup().sampleRate;

    // commandManager.registerAllCommandsForTarget(this);
    // commandManager.addTargetToCommandManager(this);

    valueTree.addListener(this);

    initGraph();
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

    clockNode = audioGraph->addNode(std::make_unique<AudioClock>(valueTree, commandManager));
    //would it be better to keep a reference to this AudioClock object to give to tracks later?

    connectNode(clockNode);

}

void MainAudio::updateGraph()
{
}

void MainAudio::addNewTrack(juce::ValueTree& node)
{
    auto* clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());
    const auto newTrack = audioGraph->addNode(std::make_unique<Track>(node, commandManager, clock));
    //add as a listener to the metronome changeBroadcaster (also remember to add initialization code in the main file, instead of here)
    trackArray.add(newTrack); //don't use this!
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
    auto clock = dynamic_cast<SPAudioProcessor*>(clockNode->getProcessor());
    clock->pauseOrResumeProcessing();
    for (auto track : trackArray)
    {
        auto processor = dynamic_cast<SPAudioProcessor*>(track->getProcessor());
        processor->pauseOrResumeProcessing();
    }
}

//ApplicationCommandTarget methods

// juce::ApplicationCommandTarget* MainAudio::getNextCommandTarget()
// {
//     return nullptr;
// }
//
// void MainAudio::getAllCommands(juce::Array<juce::CommandID> &c)
// {
//     juce::Array<juce::CommandID> commands{
//         SP_CommandID::startOrStopProcessing
//     };
//     c.addArray(commands);
// }
//
// void MainAudio::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result)
// {
//     switch (commandID)
//     {
//         case SP_CommandID::startOrStopProcessing:
//             result.setInfo("start or stop processing", "starts or stops playback", "Audio", 0);
//             result.setTicked(false);
//             result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers);
//             break;
//         default:
//             break;
//     }
// }
//
// bool MainAudio::perform(const InvocationInfo &info)
// {
//     switch (info.commandID)
//     {
//         case SP_CommandID::startOrStopProcessing:
//             pauseOrResumeProcessing();
//             break;
//         default:
//             return false;
//     }
//     return true;
// }

// ValueTreeListener methods

void MainAudio::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (parentTree.hasType(SP_ID::TRACK_BRANCH) && childWhichHasBeenAdded.hasType(SP_ID::TRACK))
    {
        addNewTrack(childWhichHasBeenAdded);
        std::cout << "audio track added from the audio module!" << std::endl;
    }
    else std::cout << "main audio parent tree does not match" << std::endl;
}




