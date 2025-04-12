
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
- std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };

*/

#pragma once
#include "SPAudioProcessor.h"

class Recorder final : public SPAudioProcessor
{
public:
    Recorder()
    {
        backgroundThread.startThread();
    }

    ~Recorder() override
    {
        stop();
    }

    //==============================================================================
    void startRecording(const juce::File &file)
    {
        stop();

        if (trackSampleRate > 0)
        {
            // Create an OutputStream to write to our destination file...
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
            {
                // Now create a WAV writer object that writes to our output stream...
                juce::WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor(fileStream.get(), trackSampleRate, 1, 16, {}, 0))
                {
                    fileStream.release();
                    // (passes responsibility for deleting the stream to the writer object that is now using it)

                    // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                    // write the data to disk on our background thread.
                    threadedWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter>(writer, backgroundThread, 32768);

                    // // Reset our recording thumbnail
                    // thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    // nextSampleNum = 0;

                    // And now, swap over our active writer pointer so that the audio callback will start using it..
                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                    std::cout << "is Recording" << std::endl;
                }
            }
        }
    }

    void stop()
    {
        // First, clear this pointer to stop the audio callback from using our writer object..
        {
            const juce::ScopedLock sl(writerLock);
            activeWriter = nullptr;
        }

        // Now we can delete the writer object. It's done in this order because the deletion could
        // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
        // the audio callback while this happens.
        threadedWriter.reset();
        std::cout << "Recording stopped" << std::endl;
    }

    bool isRecording() const
    {
        return activeWriter.load() != nullptr;
    }

    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused(samplesPerBlock);
        trackSampleRate = sampleRate;
    }

    void releaseResources() override
    {
        trackSampleRate = 0;
    }

    // void audioDeviceStopped() override
    // {
    //     sampleRate = 0;
    // }

    void processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages) override
    {
        juce::ignoreUnused(midiMessages);
        const juce::ScopedLock sl(writerLock);
        //auto numInputChannels = getTotalNumInputChannels();
        auto numOutputChannels = getTotalNumOutputChannels();

        // TO PAUSE RECORDING
        //
        // if (!paused)
        //

        if (activeWriter.load() != nullptr)
        {
            activeWriter.load()->write(buffer.getArrayOfReadPointers(), buffer.getNumSamples());


            // TO CREATE THUMBNAILS
            // // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
            // juce::AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
            // thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
            // nextSampleNum += numSamples;
        }
        // We need to clear the output buffers, in case they're full of junk..
        for (int i = 0; i < numOutputChannels; ++i)
            if (buffer.getArrayOfWritePointers()[i] != nullptr)
                juce::FloatVectorOperations::clear(buffer.getArrayOfWritePointers()[i], buffer.getNumSamples());
    }

private:
    juce::TimeSliceThread backgroundThread{"Audio Recorder Thread"};
    // the thread that will write our audio data to disk
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    // the FIFO used to buffer the incoming data
    double trackSampleRate = 0.0;
    //juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter *> activeWriter{nullptr};
};

