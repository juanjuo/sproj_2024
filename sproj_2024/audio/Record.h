//
// Created by Juan Diego on 10/4/24.
//

//


/*TODO:

- on a different thread using TimeSliceThread
- must contain audioFormatWriter::ThreadedWriter
- also contains a FileOutputStream (maybe inside record method)
- also contains a WavFormatReader (maybe inside record method)
- use locks to safely write the file in a background thread

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

