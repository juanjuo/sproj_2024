//
// Created by Juan Diego on 1/19/25.
//
#pragma once
#include "SPAudioProcessor.h"
#include <juce_audio_devices/juce_audio_devices.h>


/* AudioPlayer
 *
 * Controls Playback of files
 */
class AudioPlayer final : public SPAudioProcessor
{
public:
    explicit AudioPlayer()
    {
        formatManager.registerBasicFormats(); //WAV files manager
        lookaheadThread.startThread(juce::Thread::Priority::normal);
    }

    ~AudioPlayer() override
    {
        transportSource.setSource(nullptr);
    }

    void startPlayer()
    {
        if (!transportSource.isPlaying())
        {
            transportSource.setPosition(0);
            transportSource.start();
        }
    }

    void stopPlayer()
    {
        if (transportSource.isPlaying())
            transportSource.stop();
    }

    float getGain() const
    {
        return transportSource.getGain();
    }

    void setGain(const float gain)
    {
        transportSource.setGain(gain);
    }

    double getCurrentPlaybackPosition() const
    {
        return transportSource.getCurrentPosition();
    }

    //Audio Processor methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        setAudioSource(currentAudioFile);
        transportSource.prepareToPlay(samplesPerBlock, sampleRate);
        //setLooping(true); //for further implementation
    }

    void processBlock(juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages) override
    {
        juce::ignoreUnused(midiMessages);

        //create buffer for transportSource
        juce::AudioSourceChannelInfo bufferToFill{};
        bufferToFill.buffer = &buffer;
        bufferToFill.startSample = 0;
        bufferToFill.numSamples = buffer.getNumSamples();
        transportSource.getNextAudioBlock(bufferToFill);
    }

    void setAudioSource(juce::URL source)
    {
        if (source.isEmpty()) return;
        if (!loadURLIntoTransport(source))
        {
            // Failed to load the audio file!
            jassertfalse;
            return;
        }

        currentAudioFile = std::move(source);
    }

    //FOR FUTURE IMPLEMENTATION
    // void setLooping(const bool shouldLoop)
    // {
    //     if (currentAudioFileSource == nullptr) return;
    //     transportSource.setLooping(shouldLoop);
    //     currentAudioFileSource->setLooping(shouldLoop);
    // }
    //
    // bool isLooping() const
    // {
    //     return transportSource.isLooping();
    // }

private:
    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread lookaheadThread{"file lookahead"};

    juce::URL currentAudioFile;
    juce::AudioTransportSource transportSource;

    std::unique_ptr<juce::AudioFormatReaderSource> currentAudioFileSource;

    bool loadURLIntoTransport(const juce::URL& audioURL)
    {
        // unload the previous file source and delete it...
        transportSource.stop();
        transportSource.setSource(nullptr);
        currentAudioFileSource.reset();

        const auto source = std::make_unique<juce::URLInputSource>(audioURL);

        if (source == nullptr)
            return false;

        auto stream = juce::rawToUniquePtr(source->createInputStream());

        if (stream == nullptr)
            return false;

        auto reader = juce::rawToUniquePtr(formatManager.createReaderFor(std::move(stream)));

        if (reader == nullptr)
            return false;

        currentAudioFileSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);

        currentAudioFileSource->setLooping(true); //find a better way of doing this

        // ...and plug it into our transport source
        transportSource.setSource(currentAudioFileSource.get(),
                                  32768, // tells it to buffer this many samples ahead
                                  &lookaheadThread, // this is the background thread to use for reading-ahead
                                  currentAudioFileSource->getAudioFormatReader()->sampleRate);
        // allows for sample rate correction

        return true;
    }
};
