//
// Created by Juan Diego on 1/19/25.
//
#pragma once
#include <SPAudioProcessor.h>

class AudioPlayer final : public SPAudioProcessor
{
public:
    AudioPlayer()
    {
        formatManager.registerBasicFormats();
        setAudioResource(currentAudioFile);
        lookaheadThread.startThread(juce::Thread::Priority::normal);
    }

    ~AudioPlayer() override
    {
        transportSource.setSource(nullptr);
    }

    void setAudioResource(juce::URL resource)
    {
        if (!loadURLIntoTransport(resource))
        {
            // Failed to load the audio file!
            jassertfalse;
            return;
        }

        currentAudioFile = std::move(resource);
    }

    bool loadURLIntoTransport(const juce::URL &audioURL)
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

        // ..and plug it into our transport source
        transportSource.setSource(currentAudioFileSource.get(),
                                  32768, // tells it to buffer this many samples ahead
                                  &lookaheadThread, // this is the background thread to use for reading-ahead
                                  currentAudioFileSource->getAudioFormatReader()->sampleRate);
        // allows for sample rate correction

        return true;
    }

    void startOrStop()
    {
        if (transportSource.isPlaying())
        {
            transportSource.stop();
        } else
        {
            transportSource.setPosition(0);
            transportSource.start();
        }
    }

    //Audio Processor Methods

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        setAudioResource(currentAudioFile);
        transportSource.prepareToPlay(samplesPerBlock, sampleRate);
    }

    void releaseResources() override
    {
    }

    void processBlock(juce::AudioBuffer<float> &buffer,
                      juce::MidiBuffer &midiMessages) override
    {
        // if (currentAudioFileSource == nullptr) // for example
        // {
        //     buffer.clear();
        //     return;
        // }
        juce::AudioSourceChannelInfo bufferToFill{};
        bufferToFill.buffer      = &buffer;
        bufferToFill.startSample = 0;
        bufferToFill.numSamples  = buffer.getNumSamples();
        transportSource.getNextAudioBlock (bufferToFill);
    }

private:
    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread lookaheadThread {"file lookahead"};

    juce::URL currentAudioFile {juce::File("/Users/juan/Desktop/you_look_so_good.mp3")};
    juce::AudioTransportSource transportSource;

    std::unique_ptr<juce::AudioFormatReaderSource> currentAudioFileSource;
};
