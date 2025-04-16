
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

/*  Recorder
 *
 *  This class is in charged of all recording logic
 */
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
                    threadedWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter>(writer, backgroundThread, 32768);

                    // for further implementations (thumbnail)
                    // thumbnail.reset (writer->getNumChannels(), writer->getSampleRate());
                    // nextSampleNum = 0;

                    // Swap pointers to be used by audioCallback
                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                    std::cout << "is Recording" << std::endl;
                }
            }
        }
    }

    void stop()
    {
        // First, clear this pointer to stop the audio callback from using our writer object
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

    // AudioProcessor methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused(samplesPerBlock);
        trackSampleRate = sampleRate;
    }

    void releaseResources() override
    {
        trackSampleRate = 0;
    }

    void processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages) override
    {
        juce::ignoreUnused(midiMessages);
        const juce::ScopedLock sl(writerLock);
        //auto numInputChannels = getTotalNumInputChannels();
        auto numOutputChannels = getTotalNumOutputChannels();

        if (activeWriter.load() != nullptr)
        {
            activeWriter.load()->write(buffer.getArrayOfReadPointers(), buffer.getNumSamples());

            // TO CREATE THUMBNAILS
            // // Create an AudioBuffer to wrap our incoming data, note that this does no allocations or copies, it simply references our input data
            // juce::AudioBuffer<float> buffer (const_cast<float**> (inputChannelData), thumbnail.getNumChannels(), numSamples);
            // thumbnail.addBlock (nextSampleNum, buffer, 0, numSamples);
            // nextSampleNum += numSamples;
        }
        for (int i = 0; i < numOutputChannels; ++i)
            if (buffer.getArrayOfWritePointers()[i] != nullptr)
                juce::FloatVectorOperations::clear(buffer.getArrayOfWritePointers()[i], buffer.getNumSamples());
    }

private:
    juce::TimeSliceThread backgroundThread{"Audio Recorder Thread"}; //thread that writes data to disk
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter; // FIFO used to buffer incoming data
    double trackSampleRate = 0.0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter *> activeWriter{nullptr};
};

