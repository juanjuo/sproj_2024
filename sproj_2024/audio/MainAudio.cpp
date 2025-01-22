//
// Created by Juan Diego on 10/29/24.
//

#include "MainAudio.h"

MainAudio::MainAudio(juce::ValueTree v, SPCommandManager &manager, juce::AudioDeviceManager &audioManager)
    : audioGraph(new juce::AudioProcessorGraph()), valueTree(v), commandManager(manager), deviceManager(audioManager)
{
    audioGraph->enableAllBuses();

    deviceManager.initialiseWithDefaultDevices(1, 1); //1 inputs, 1 output
    deviceManager.addAudioCallback(&audioPlayer);

    initGraph();

    audioPlayer.setProcessor(audioGraph.get());
}

MainAudio::~MainAudio()
{
    deviceManager.removeAudioCallback(&audioPlayer);
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



