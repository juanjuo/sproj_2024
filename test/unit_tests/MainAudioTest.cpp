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
// #include <Track.h>
// #include <FreeDeckGUI.h>
// #include <SPCommandManager.h>
// #include <AudioClock.h>


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

// TEST(MainAudioTest, AudioTrack)
// {
//     juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
//     FreeDeckGUI deckGui {tree};
//     SPCommandManager commandManager;
//     AudioClock clock {tree};
//     Track track {tree, tree, commandManager, (&clock)};
// }