//
// Created by Juan Diego on 10/29/24.
//

/*
TODO:

Each track is an AudioProcessor, which is able to record audio form an Audio Device.
the AudioProcessor then should save all the recorded audio in a BUFFER, which will
get played accordingly (how is the AudioProcessor then gonna play the recorded stuff?)

each of these tracks are then assigned a AudioProcessorPlayer and then added to an AudioGraph
which is in charged of mixing all of the tracks together

ADD MIDI

 */

#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_audio_utils/juce_audio_utils.h"
#include "juce_audio_devices/juce_audio_devices.h"
#include <SPCommandManager.h>
#include "Track.h"

/*  MainAudio
 *
 *  This class holds a list of tracks, representing their processing in an AudioGraph
 */
class MainAudio final : public juce::ValueTree::Listener
{
public:

    MainAudio(juce::ValueTree v, SPCommandManager& manager, juce::AudioDeviceManager& audioManager);

    ~MainAudio() override;

    void initGraph();

    void addNewTrack(juce::ValueTree& node);

    void connectNode(const juce::AudioProcessorGraph::Node::Ptr& node) const;

    void pauseOrResumeProcessing();

    //ValueTreeListener methods
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;

private:
    juce::ValueTree valueTree;

    juce::AudioProcessorGraph::Node::Ptr inputNode;
    juce::AudioProcessorGraph::Node::Ptr outputNode;
    juce::AudioProcessorGraph::Node::Ptr clockNode;
    std::unique_ptr<juce::AudioProcessorGraph> audioGraph;
    juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node> trackArray;

    juce::AudioDeviceManager& deviceManager;
    std::unique_ptr<juce::AudioProcessorPlayer> audioPlayer = std::make_unique<juce::AudioProcessorPlayer>();

    double baseSampleRate;

    SPCommandManager& commandManager;
};
