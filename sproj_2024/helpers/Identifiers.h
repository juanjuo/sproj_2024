//
// Created by Juan Diego on 10/23/24.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace SP_ID
{
    #define DECLARE_ID(name) const juce::Identifier name (#name);

    //names for branches
    DECLARE_ID (MAIN_BRANCH)
    DECLARE_ID (TRACK_BRANCH)
    DECLARE_ID (METRONOME_BRANCH)

    //values
    DECLARE_ID (numerator)
    DECLARE_ID (denominator)
    DECLARE_ID (bpm)
    DECLARE_ID (playback_value)

    #undef DECLARE_ID
}

enum SP_CommandID
{
    print = 1,
    show_audio_device_menu,
    record,
    play,
    stopProcessing,
    createNewTrack,
    createNewDummyClip
};

// helper function to move child components around, I might not need this
// namespace SP
// {
//     static void moveChildComponent(juce::Component* childComponent, juce::Component* oldParent, juce::Component* newParent)
//     {
//         newParent->addAndMakeVisible(childComponent);
//         oldParent->removeChildComponent(childComponent);
//     }
// }

