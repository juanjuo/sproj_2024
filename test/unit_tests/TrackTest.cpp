//
// Created by Juan Diego on 4/12/25.
//

#include <gtest/gtest.h>
#include "sproj_2024_lib/audio/Track.h"
#include <sproj_2024_lib/gui/SPCommandManager.h>
#include <sproj_2024_lib/audio/AudioClock.h>
#include <sproj_2024_lib/audio/MainAudio.h>

TEST(TrackTest, Track_PauseOrResumeProcessing_1_Test)
{
    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    mainAudio.addNewTrack(tree);
    mainAudio.pauseOrResumeProcessing();

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_FALSE(track->getIsPaused());
}

TEST(TrackTest, Track_PauseOrResumeProcessing_2_Test)
{
    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    mainAudio.addNewTrack(tree);
    mainAudio.pauseOrResumeProcessing();

    mainAudio.pauseOrResumeProcessing();

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_TRUE(track->getIsPaused());
}

//ValueTree Tests
/*
 * Parsing ValueTrees into clipData
 *
 */
TEST(TrackTest, Track_ValueTree_1_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 8;
    constexpr int length = 8;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, " ", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    auto clipData = track->getClipData();

    ASSERT_TRUE(clipData.size() == 1);
    ASSERT_EQ(clipData[0].start, start);
    ASSERT_EQ(clipData[0].end, end);
    ASSERT_EQ(clipData[0].ID, trackClipNode.getProperty(SP_ID::U_ID));
    ASSERT_NE(trackClipNode.getProperty(SP_ID::clip_filepath), " ");
}

/*
 * Parsing multiple ValueTrees into clipData
 *
 */
TEST(TrackTest, Track_ValueTree_2_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 8;
    constexpr int length = 8;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, " ", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree2
    constexpr int start2 = 100;
    constexpr int end2 = 108;
    constexpr int length2 = 8;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode2);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "some/other/file", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    //Initialize ClipTree3
    constexpr int start3 = 159;
    constexpr int end3 = 0;
    constexpr int length3 = 1;
    juce::ValueTree trackClipNode3(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode3);
    trackClipNode3.setProperty(SP_ID::clip_start_value, start3, nullptr);
    trackClipNode3.setProperty(SP_ID::clip_end_value, end3, nullptr);
    trackClipNode3.setProperty(SP_ID::clip_filepath, "example/file/path", nullptr);
    trackClipNode3.setProperty(SP_ID::clip_length_value, length3, nullptr);
    trackClipNode3.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode3, nullptr);

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    auto clipData = track->getClipData();

    ASSERT_TRUE(clipData.size() == 3);
    ASSERT_EQ(clipData[0].start, start);
    ASSERT_EQ(clipData[1].start, start2);
    ASSERT_EQ(clipData[2].start, start3);

    ASSERT_EQ(clipData[0].end, end);
    ASSERT_EQ(clipData[1].end, end2);
    ASSERT_EQ(clipData[2].end, end3);

    ASSERT_EQ(clipData[0].ID, trackClipNode.getProperty(SP_ID::U_ID));
    ASSERT_EQ(clipData[1].ID, trackClipNode2.getProperty(SP_ID::U_ID));
    ASSERT_EQ(clipData[2].ID, trackClipNode3.getProperty(SP_ID::U_ID));

    ASSERT_NE(trackClipNode.getProperty(SP_ID::clip_filepath), " ");
    ASSERT_EQ(trackClipNode2.getProperty(SP_ID::clip_filepath), "some/other/file");
    ASSERT_EQ(trackClipNode3.getProperty(SP_ID::clip_filepath), "example/file/path");
}

/*
 * Updating file path inside valueTree from Track
 *
 */
TEST(TrackTest, Track_ValueTree_3_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 8;
    constexpr int length = 8;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    auto clipData = track->getClipData();

    ASSERT_TRUE(clipData.size() == 1);
    ASSERT_EQ(trackClipNode.getProperty(SP_ID::clip_filepath), "/test/filepath/example");
}

/*
 * Updating gain
 *
 */
TEST(TrackTest, Track_ValueTree_4_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    //change gain
    trackTree.setProperty(SP_ID::track_gain, 2, nullptr);

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    auto player = track->getPlayer();

    ASSERT_TRUE(player->getGain() == 2);
}


//Scheduler Tests
/*
 * Scheduling currentBeat updates after sendChangeMessage (with no clips)
 */
TEST(TrackTest, Track_Scheduler_1_Test)
{
    //Initialize Main Audio
    juce::ValueTree tree = juce::ValueTree {"dummy_tree"};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {tree, commandManager, deviceManager};

    mainAudio.addNewTrack(tree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_EQ(track->getCurrentBeat(), 1);
}

/*
 * Scheduling start recording test
 */
TEST(TrackTest, Track_Scheduler_2_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_TRUE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);
}

/*
 * Scheduling stop recording test
 */
TEST(TrackTest, Track_Scheduler_3_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_TRUE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);
}

/*
 * Scheduling start playback test
 */
TEST(TrackTest, Track_Scheduler_4_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);
}

/*
 * Scheduling stop playback test
 */
TEST(TrackTest, Track_Scheduler_5_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);
}

/*
 * Scheduling clipData deletes when end == 0
 */
TEST(TrackTest, Track_Scheduler_6_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    auto clipData = track->getClipData();

    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling currentBeat wraps around max num of beats
 */
TEST(TrackTest, Track_Scheduler_7_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 160; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 0);
}

/*
 * Scheduling currentBeat wraps around max num of beats manual
 */
TEST(TrackTest, Track_Scheduler_8_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 159; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 159);

    clock->sendSynchronousChangeMessage();

    ASSERT_EQ(track->getCurrentBeat(), 0);
}

/*
 * Scheduling recording start and end wrap around max beats + deletes clipData
 */
TEST(TrackTest, Track_Scheduler_9_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree
    constexpr int start = 159;
    constexpr int end = 0;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 159; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 159);
    ASSERT_FALSE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());

    clock->sendSynchronousChangeMessage();

    ASSERT_EQ(track->getCurrentBeat(), 0);
    ASSERT_TRUE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());

    clock->sendSynchronousChangeMessage();

    ASSERT_EQ(track->getCurrentBeat(), 1);
    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling recording and playback back to back
 */
TEST(TrackTest, Track_Scheduler_10_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree 1
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 1;
    constexpr int end2 = 2;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    trackClipNode2.setProperty(SP_ID::U_ID, trackClipNode.getProperty(SP_ID::U_ID), nullptr);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 3);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling recording and playback back to back on wrap around beats
 */
TEST(TrackTest, Track_Scheduler_11_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();
    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());
    ASSERT_EQ(trackList.size(), 1);

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 60; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    //Initialize ClipTree 1
    constexpr int start = 159;
    constexpr int end = 0;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 0;
    constexpr int end2 = 1;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    trackClipNode2.setProperty(SP_ID::U_ID, trackClipNode.getProperty(SP_ID::U_ID), nullptr);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    ASSERT_EQ(track->getCurrentBeat(), 60);
    for (int i = 0; i < 99; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 159);
    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());

    clock->sendSynchronousChangeMessage();

    ASSERT_EQ(track->getCurrentBeat(), 0);
    ASSERT_TRUE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling recording back to back to different sources
 */
TEST(TrackTest, Track_Scheduler_12_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree 1
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 1;
    constexpr int end2 = 2;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/some/other/filepath", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    //ASSERT_EQ(track->getLastRecording(), "/test/filepath/example");

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_TRUE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    //ASSERT_EQ(track->getLastRecording(), "/some/other/filepath");

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 3);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling playback back to back to different sources
 */
TEST(TrackTest, Track_Scheduler_13_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree 1
    constexpr int start = 0;
    constexpr int end = 1;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 1;
    constexpr int end2 = 2;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/some/other/filepath", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isRecording());
    ASSERT_TRUE(track->isPlaying());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 3);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling recording back to back to different sources wrap around beats
 */
TEST(TrackTest, Track_Scheduler_14_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree 1
    constexpr int start = 159;
    constexpr int end = 0;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 0;
    constexpr int end2 = 1;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/some/other/filepath", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 0, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 159; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 159);
    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isRecording());
    ASSERT_FALSE(track->isPlaying());
    ASSERT_EQ(track->getCurrentBeat(), 0);

    //ASSERT_EQ(track->getLastRecording(), "/test/filepath/example");

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_TRUE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    //ASSERT_EQ(track->getLastRecording(), "/some/other/filepath");

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}

/*
 * Scheduling playback back to back to different sources wrap around beats
 */
TEST(TrackTest, Track_Scheduler_15_Test)
{
    //Initialize Main Audio
    juce::ValueTree mainAudioTree = juce::ValueTree {SP_ID::TRACK_BRANCH};
    juce::ValueTree trackTree = juce::ValueTree {SP_ID::TRACK};
    SPCommandManager commandManager;
    juce::AudioDeviceManager deviceManager;
    MainAudio mainAudio {mainAudioTree, commandManager, deviceManager};

    mainAudio.addNewTrack(trackTree);

    auto clockNode = mainAudio.getAudioClockNode();
    auto clock = dynamic_cast<AudioClock*>(clockNode->getProcessor());

    auto trackList = mainAudio.getAudioTrackArray();

    ASSERT_EQ(trackList.size(), 1);

    //Initialize ClipTree 1
    constexpr int start = 159;
    constexpr int end = 0;
    constexpr int length = 1;
    juce::ValueTree trackClipNode(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode.setProperty(SP_ID::clip_start_value, start, nullptr);
    trackClipNode.setProperty(SP_ID::clip_end_value, end, nullptr);
    trackClipNode.setProperty(SP_ID::clip_filepath, "/test/filepath/example", nullptr);
    trackClipNode.setProperty(SP_ID::clip_length_value, length, nullptr);
    trackClipNode.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode, nullptr);

    //Initialize ClipTree 2
    constexpr int start2 = 0;
    constexpr int end2 = 1;
    constexpr int length2 = 1;
    juce::ValueTree trackClipNode2(SP_ID::CLIP); //add ValueTree to track
    SP::createNewID(trackClipNode);
    trackClipNode2.setProperty(SP_ID::clip_start_value, start2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_end_value, end2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_filepath, "/some/other/filepath", nullptr);
    trackClipNode2.setProperty(SP_ID::clip_length_value, length2, nullptr);
    trackClipNode2.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
    trackTree.appendChild(trackClipNode2, nullptr);

    auto trackNode = trackList[0];
    auto track = dynamic_cast<Track*>(trackNode->getProcessor());

    ASSERT_EQ(track->getCurrentBeat(), 0);

    for (int i = 0; i < 159; i++)
    {
        clock->sendSynchronousChangeMessage();
    }

    ASSERT_EQ(track->getCurrentBeat(), 159);
    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isRecording());
    ASSERT_TRUE(track->isPlaying());
    ASSERT_EQ(track->getCurrentBeat(), 0);

    clock->sendSynchronousChangeMessage();

    ASSERT_TRUE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 1);

    clock->sendSynchronousChangeMessage();

    ASSERT_FALSE(track->isPlaying());
    ASSERT_FALSE(track->isRecording());
    ASSERT_EQ(track->getCurrentBeat(), 2);

    auto clipData = track->getClipData();
    ASSERT_EQ(clipData.size(), 0);
}