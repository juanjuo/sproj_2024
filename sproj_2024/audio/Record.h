//
// Created by Juan Diego on 10/4/24.
//

//


/*TODO:

There should be one of this classes getting info from a single AudioDevice, which might
have multiple audio inputs which can be activated/deactivated to save their input into
a buffer. (Read about BUFFERS in JUCE)

This class should also be able to

*/

#pragma once

//#include <juce_audio_buffer/juce_audio_buffer.h>
//#include <juce_audio_source/juce_audio_source.h>

class Record
{
public:

    Record();
    ~Record();

    void startRecording();
    void stopRecording();
    void cropRecording();

private:
    //juce::AudioDeviceManager inputDeviceManager; //used to access the inputs of a device

};

