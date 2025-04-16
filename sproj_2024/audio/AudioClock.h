//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include "SPAudioProcessor.h"
#include <random>
#include <helpers.h>

/* WavetableOscillator
 *
 * generates a wavetable for a sinewave to use as a metronome sound.
 */
class WavetableOscillator
{
public:
    explicit WavetableOscillator (const juce::AudioSampleBuffer& wavetableToUse)
        : wavetable (wavetableToUse),
          tableSize (wavetable.getNumSamples() - 1)
    {
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


/* AudioClock
 *
 * Works as a metronome and triggers updates for Track classes
 */
class AudioClock final : public SPAudioProcessor,
                         public juce::ValueTree::Listener,
                         public juce::ChangeBroadcaster
{
public:

    explicit AudioClock(const juce::ValueTree& v)
    : clockValueTree(v.getChildWithName(SP_ID::METRONOME_BRANCH)), intervalVal(5000), counterVal(0), noteLength(2000), clockSampleRate(44100)/*, scheduler(sch)*/
    {
        clockValueTree.addListener(this);
        createWavetable();
    }

    //AudioProcessor methods
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused(samplesPerBlock);

        clockSampleRate = sampleRate;

        intervalVal = milliToSamples(BPMToMilli(120), sampleRate); //length of beat

        counterVal = 0; //reset counter

        //initialize wavetable
        sineTone = new WavetableOscillator (sineTable);
        int frequency = 440.0;
        sineTone->setFrequency ((float) frequency, (float) sampleRate);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessage) override
    {
        juce::ignoreUnused(midiMessage);

        if (!isPaused)
        {
            juce::ScopedNoDenormals noDenormals;
            //auto totalNumInputChannels  = getTotalNumInputChannels(); //not necessary! but available for use
            auto totalNumOutputChannels = getTotalNumOutputChannels();

            // for all the samples in the buffer
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                if (++counterVal >= intervalVal)// reset counter
                {
                    counterVal = 0;
                    if (updateHasBeenSent)
                    {
                        updateHasBeenSent = false;
                    }
                }
                if (counterVal < noteLength)//  send update
                {
                    if (!updateHasBeenSent)
                    {
                        sendChangeMessage();
                        updateHasBeenSent = true;
                    }
                    for (int channel = 0; channel < totalNumOutputChannels; channel++) // for all channels
                    {
                        auto* channelData = buffer.getWritePointer (channel);
                        auto levelSample = sineTone->getNextSample() * level; // write sine tone
                        channelData[sample] = levelSample;
                    }
                }
                else
                {
                    for (int channel = 0; channel < totalNumOutputChannels; channel++) //write nothing
                    {
                        auto* channelData = buffer.getWritePointer (channel);
                        channelData[sample] = 0;

                    }
                }
            }
        }
        else
        {
            buffer.clear();
        }
    }


    //Async updater methods
    /*
     * Update BPM interval (must be async since its being used by real-time thread)
     */
    void handleAsyncUpdate() override
    {
        {
            int bpm = clockValueTree.getProperty(SP_ID::metronome_bpm);
            intervalVal = milliToSamples(BPMToMilli(bpm), clockSampleRate);
        }
    }

    //ValueTree Listener methods
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
        if (treeWhosePropertyHasChanged.getType() == SP_ID::METRONOME_BRANCH)
            if (property == SP_ID::metronome_bpm)
                triggerAsyncUpdate(); //update tracks
            if (property == SP_ID::metronome_grouping) //to be implemented
                std::cout << "bar grouping changed" << std::endl;
            if (property == SP_ID::metronome_gain) //change gain
                level = treeWhosePropertyHasChanged.getProperty(property);
    }

    void createWavetable()
    {
        sineTable.setSize (1, (int) tableSize + 1);
        auto* samples = sineTable.getWritePointer (0);

        auto angleDelta = juce::MathConstants<double>::twoPi / (double) (tableSize - 1);
        auto currentAngle = 0.0;

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin (currentAngle);
            samples[i] = (float) sample;
            currentAngle += angleDelta;
        }
        samples[tableSize] = samples[0];
    }

    void setNumerator(int numerator)
    {
    numeratorVal = numerator;
    }

    void setDenominator(int denominator)
    {
        denominatorVal = denominator;
    }

    void setInterval(const int interval)
    {
        intervalVal = interval;
    }

    void setBPM(int bpm)
    {
        bpmVal = bpm;
    }

    static int BPMToMilli(const int bpm)
    {
        if (bpm <= 0) {
            std::cerr << "BPM must be a positive integer." << std::endl;
            return -1; // Indicate an error
        }
        return static_cast<int> (60000.0f / bpm); // Convert BPM to milliseconds
    }

    static int milliToSamples(const int milliseconds, const double sampleRate)
    {
        const double samples = sampleRate * (milliseconds / 1000.0);
        return static_cast<int> (samples);
    }


    static float milliToBPM(float milliseconds)
    {
        if (milliseconds <= 0) {
            std::cerr << "Milliseconds must be a positive value." << std::endl;
            return -1; // Indicate an error
        }
        return 60000.0f / milliseconds; // Convert milliseconds to BPM
    }

private:
    juce::ValueTree clockValueTree;

    bool updateHasBeenSent = false;

    int bpmVal;
    int numeratorVal;
    int denominatorVal;

    int intervalVal;
    int counterVal;

    int noteLength;

    double clockSampleRate;

    //wavetable
    const unsigned int tableSize = 1 << 7;
    std::atomic<float> level = 0.5f; //must be thread safe
    juce::AudioSampleBuffer sineTable;
    WavetableOscillator *sineTone{};
};

