//
// Created by Juan Diego on 10/29/24.
//

#include "MainAudio.h"

MainAudio::MainAudio(juce::ValueTree v) : audioGraph(new juce::AudioProcessorGraph()), mainAudioValueTree(v)
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

    clock = audioGraph->addNode(std::make_unique<Clock>(mainAudioValueTree));

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
                                                { clock->nodeID, channel } });
        audioGraph->addConnection({{clock->nodeID, channel},
                                                            {outputNode->nodeID, channel} });
    }
}


