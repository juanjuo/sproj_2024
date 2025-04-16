//
// Created by Juan Diego on 4/12/25.
//

#include <gtest/gtest.h>
#include "sproj_2024_lib/audio/Track.h"
#include <sproj_2024_lib/gui/FreeDeckGUI.h>
#include <sproj_2024_lib/gui/SPCommandManager.h>
#include <sproj_2024_lib/audio/AudioClock.h>
#include <sproj_2024_lib/audio/MainAudio.h>

TEST(AudioClockTest, Clock_PauseOrResumeProcessing_1_Test)
{
    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    auto clockNode = mainAudio.getAudioClockNode();

    auto* clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    //Pause Clock
    mainAudio.pauseOrResumeProcessing();

    ASSERT_FALSE(clock->getIsPaused());
}

TEST(AudioClockTest, Clock_PauseOrResumeProcessing_2_Test)
{
    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    //Pause Clock

    auto clockNode = mainAudio.getAudioClockNode();

    auto* clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    mainAudio.pauseOrResumeProcessing();

    mainAudio.pauseOrResumeProcessing();

    ASSERT_TRUE(clock->getIsPaused());
}

