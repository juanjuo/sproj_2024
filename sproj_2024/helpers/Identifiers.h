//
// Created by Juan Diego on 10/23/24.
//

#pragma once

namespace SP_ID
{
    #define DECLARE_ID(name) const juce::Identifier name (#name);

    //names for branches
    DECLARE_ID (MAIN_BRANCH)
    DECLARE_ID (name)

    //values
    DECLARE_ID (numerator)
    DECLARE_ID (denominator)
    DECLARE_ID (bpm)

    #undef DECLARE_ID
}

enum SP_CommandID
{
    print = 1,
    show_audio_device_menu,
    record
};

