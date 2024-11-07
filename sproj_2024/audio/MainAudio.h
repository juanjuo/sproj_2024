//
// Created by Juan Diego on 10/29/24.
//

/*
TODO:

Each track is a AudioProcessor, which is able to record audio form a Audio Device.
the AudioProcessor then should save all the recorded audio in a BUFFER, which will
get played accordingly (how is the AudioProcessor then gonna play the recorded stuff?)

each of these tracks are then assigned a AudioProcessorPlayer and then added to an AudioGraph
which is in charged of mixing all of the tracks together

Maybe the main file should be an DeviceManager? so that then you can turn on/off the audio callbacks or the audio graph
and then everything else builds on top.


ADD MIDI


 */

#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_audio_devices/juce_audio_devices.h"
#include <Clock.h>

class MainAudio
{
public:
    MainAudio(juce::ValueTree v);

    ~MainAudio();

    void initGraph();

    void updateGraph();

    void connectAudioNodes();

private:

    juce::ValueTree mainAudioValueTree;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;

    juce::AudioProcessorGraph::Node::Ptr clock;

    std::unique_ptr<juce::AudioProcessorGraph> audioGraph;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer audioPlayer;


};
