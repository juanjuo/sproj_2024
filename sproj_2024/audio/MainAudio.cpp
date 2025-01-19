//
// Created by Juan Diego on 10/29/24.
//

#include "MainAudio.h"

MainAudio::MainAudio(juce::ValueTree v, SPCommandManager& manager, juce::AudioDeviceManager& audioManager)
    : audioGraph(new juce::AudioProcessorGraph()), valueTree(v), commandManager(manager), deviceManager(audioManager)
{
    audioGraph->enableAllBuses();

    deviceManager.initialiseWithDefaultDevices(1, 1); //1 inputs, 1 output
    deviceManager.addAudioCallback(&audioPlayer);

    initGraph();

    audioPlayer.setProcessor(audioGraph.get());
    //
    // commandManager.registerAllCommandsForTarget (this); //3 REGISTERING COMMANDS FOR THIS TARGET
    //
    // // this ensures that commands invoked on the DemoRunner application are correctly
    // // forwarded to this demo
    // commandManager.setFirstCommandTarget (nullptr);
}

MainAudio::~MainAudio()
{
    deviceManager.removeAudioCallback(&audioPlayer);

}

// //ApplicationCommandTarget methods
// MainAudio::ApplicationCommandTarget *getNextCommandTarget()
// {
//     return nullptr;
// }
//
// void MainAudio::getAllCommands(juce::Array<juce::CommandID> &c)
// {
//     juce::Array<juce::CommandID> commands{
//         SP_CommandID::print
//     };
//     c.addArray(commands);
// }
//
// void MainAudio::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result)
// {
//     switch (commandID)
//     {
//         case SP_CommandID::print:
//             result.setInfo("Print", "Prints something to the console", "Print", 0);
//         result.setTicked(false);
//         result.addDefaultKeypress('p', juce::ModifierKeys::commandModifier);
//         break;
//         default:
//             break;
//     }
// }
//
// bool MainAudio::perform(const InvocationInfo &info)
// {
//     switch (info.commandID)
//     {
//         case SP_CommandID::print:
//             std::cerr << "PRINTING TO THE SCREEN FROM AUDIO" << std::endl;
//         break;
//         default:
//             return false;
//     }
//
//     return true;
// }
// //

void MainAudio::initGraph()
{
    audioGraph->clear();

    inputNode = audioGraph->addNode(
        std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>
        (juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode));

    outputNode = audioGraph->addNode(
        std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>
        (juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));

    recorder = audioGraph->addNode(std::make_unique<Track>(valueTree, commandManager));

    //initialize metronome
    metronome = audioGraph->addNode(std::make_unique<Clock>(valueTree));

    connectAudioNodes();

}

void MainAudio::updateGraph()
{

}

void MainAudio::connectAudioNodes()
{
    for (int channel = 0; channel < 2; ++channel)
    {
        audioGraph->addConnection({ { inputNode->nodeID,  channel},
                                                { metronome->nodeID, channel } });
        audioGraph->addConnection({{metronome->nodeID, channel},
                                                {recorder->nodeID, channel} });
        audioGraph->addConnection({{recorder->nodeID, channel},
                                                {outputNode->nodeID, channel} });
    }
}


