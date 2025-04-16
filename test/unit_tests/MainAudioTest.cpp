//
// Created by Juan Diego on 4/9/25.
//

#include <gtest/gtest.h>
#include "sproj_2024_lib/audio/Track.h"
#include <sproj_2024_lib/gui/FreeDeckGUI.h>
#include <sproj_2024_lib/gui/SPCommandManager.h>
#include <sproj_2024_lib/audio/AudioClock.h>
#include <sproj_2024_lib/audio/MainAudio.h>

TEST(MainAudioTest, MainAudio_ConnectNode_Test)
{
    //initialize Track
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    AudioClock clock {tree};

    //Initialize Main Audio
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    //add new track
    auto audioGraph = mainAudio.getAudioGraph();
    auto trackNode = audioGraph->addNode(std::make_unique<Track>(tree, tree, commandManager, (&clock)));

    mainAudio.connectNode(trackNode);

    auto inputNode = mainAudio.getInputNode();
    auto outputNode = mainAudio.getOutputNode();

    ASSERT_TRUE(audioGraph->isConnected(inputNode->nodeID, trackNode->nodeID));
    ASSERT_TRUE(audioGraph->isConnected( trackNode->nodeID, outputNode->nodeID));
}

TEST(MainAudioTest, MainAudio_AddNewTrack_1_Test)
{
    //initialize Track
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;

    //Initialize Main Audio
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    //add new track
    mainAudio.addNewTrack(tree);

    auto trackArray = mainAudio.getAudioTrackArray();

    ASSERT_TRUE(trackArray.size() == 1);
}

TEST(MainAudioTest, MainAudio_AddNewTrack_2_Test)
{
    //initialize Track
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;

    //Initialize Main Audio
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    //add new track
    for (int i = 0; i < 6; i++)
        mainAudio.addNewTrack(tree);

    auto trackArray = mainAudio.getAudioTrackArray();

    ASSERT_TRUE(trackArray.size() == 6);
}

// ValueTree
/*
 * Add new Track via ValueTree
 *
 */
TEST(MainAudioTest, MainAudio_AddNewTrack_3_Test)
{
    //initialize Track
    juce::ValueTree tree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    SPCommandManager commandManager;

    //Initialize Main Audio
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    //add new track via value tree
    auto newTree = juce::ValueTree {SP_ID::TRACK};
    tree.appendChild(newTree, nullptr);

    SP::printVT(tree);

    auto trackArray = mainAudio.getAudioTrackArray();

    ASSERT_TRUE(trackArray.size() == 1);
}

TEST(MainAudioTest, MainAudio_PauseOrResumeProcessing_1_Test)
{
    constexpr int numOfTracks = 6;

    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    for (int i = 0; i < numOfTracks; i++)
        mainAudio.addNewTrack(tree);

    mainAudio.pauseOrResumeProcessing();

    auto trackList = mainAudio.getAudioTrackArray();

    for (int i = 0; i < numOfTracks; i++)
    {
        auto trackNode = trackList[i];
        auto track = dynamic_cast<Track*>(trackNode->getProcessor());
        ASSERT_FALSE(track->getIsPaused());
    }
}

TEST(MainAudioTest, MainAudio_PauseOrResumeProcessing_2_Test)
{
    constexpr int numOfTracks = 6;

    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    for (int i = 0; i < numOfTracks; i++)
        mainAudio.addNewTrack(tree);

    mainAudio.pauseOrResumeProcessing();

    mainAudio.pauseOrResumeProcessing();


    auto trackList = mainAudio.getAudioTrackArray();

    for (int i = 0; i < numOfTracks; i++)
    {
        auto trackNode = trackList[i];
        auto track = dynamic_cast<Track*>(trackNode->getProcessor());
        ASSERT_TRUE(track->getIsPaused());
    }
}

