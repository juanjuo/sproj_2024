//
// Created by Juan Diego on 4/9/25.
//

// #include <MainAudio.h>
// #include "juce_audio_devices/juce_audio_devices.h"
//
// class MainAudioTest  : public juce::UnitTest
// {
// public:
//     MainAudioTest()  : UnitTest ("Foobar testing") {}
//
//     void runTest() override
//     {
//         beginTest ("Part 1");
//
//         beginTest ("Part 2");
//     }
// };
//
// // Creating a static instance will automatically add the instance to the array
// // returned by UnitTest::getAllTests(), so the test will be included when you call
// // UnitTestRunner::runAllTests()
// static MainAudioTest test;

#include <gtest/gtest.h>
#include "sproj_2024_lib/audio/Track.h"
#include <sproj_2024_lib/gui/FreeDeckGUI.h>
#include <sproj_2024_lib/gui/SPCommandManager.h>
#include <sproj_2024_lib/audio/AudioClock.h>
#include <sproj_2024_lib/audio/MainAudio.h>


TEST(MainAudioTest, Test1)
{

    ASSERT_FALSE(false);

    EXPECT_STRNE("Hello", "Sailor");

    EXPECT_EQ(7 * 6, 42);
}

TEST(MainAudioTest, Test2)
{
    EXPECT_EQ(2 * 2, 4);
}

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