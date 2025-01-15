//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include <SPAudioProcessor.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <random>
#include <Identifiers.h>

class Clock : public SPAudioProcessor,
              public juce::ValueTree::Listener
{
public:

    explicit Clock(juce::ValueTree v);

    //AudioProcessor
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void releaseResources() override;

    void reset() override;
    //


    //ValueTree Listener
    void handleAsyncUpdate() override;

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

    //

    // CLock
    void createWavetable();

    void setNumerator(int numerator); //passing by reference since these values won't be modified

    void setDenominator(int denominator);

    void setInterval(float interval);

    void setBPM(int bpm);

    float BPMToMilli(int bpm);

    int milliToSamples(int milliseconds, double sampleRate);

    float milliToBPM(float milliseconds);

    class WavetableOscillator
    {
    public:
        WavetableOscillator (const juce::AudioSampleBuffer& wavetableToUse)
            : wavetable (wavetableToUse),
              tableSize (wavetable.getNumSamples() - 1)
        {
            //jassert (wavetable.getNumChannels() == 1);
        }

        void setFrequency (float frequency, float sampleRate)
        {
            auto tableSizeOverSampleRate = (float) tableSize / sampleRate;
            tableDelta = frequency * tableSizeOverSampleRate;
        }

            float getNextSample() noexcept
        {
            auto index0 = (unsigned int) currentIndex;
            auto index1 = index0 + 1;

            auto frac = currentIndex - (float) index0;

            auto* table = wavetable.getReadPointer (0);
            auto value0 = table[index0];
            auto value1 = table[index1];

            auto currentSample = value0 + frac * (value1 - value0);

            if ((currentIndex += tableDelta) > (float) tableSize)
                currentIndex -= (float) tableSize;

            return currentSample;
        }

    private:
        const juce::AudioSampleBuffer& wavetable;
        const int tableSize;
        float currentIndex = 0.0f, tableDelta = 0.0f;
    };


private:

    juce::ValueTree clockValueTree;


    int bpm_val{};
    int numerator_val{};
    int denominator_val{};

    int interval_val;
    int counter_val;

    int note_length;

    double clockSampleRate;

    //wavetable
    const unsigned int tableSize = 1 << 7;
    float level = 0.5f;
    juce::AudioSampleBuffer sineTable;
    WavetableOscillator *sineTone;



    std::random_device rd;
    std::default_random_engine generator;

    std::uniform_real_distribution<float> distribution;

};

