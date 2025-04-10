//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include <SPAudioProcessor.h>
#include <SPCommandManager.h>
#include <random>
#include <helpers.h>

class WavetableOscillator
{
public:
    explicit WavetableOscillator (const juce::AudioSampleBuffer& wavetableToUse)
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


class AudioClock final : public SPAudioProcessor,
                         public juce::ValueTree::Listener,
                         public juce::ChangeBroadcaster
{
public:

    explicit AudioClock(const juce::ValueTree& v)
    : clockValueTree(v.getChildWithName(SP_ID::METRONOME_BRANCH)), interval_val(5000), counter_val(0), note_length(2000), clockSampleRate(44100)/*, scheduler(sch)*/
    {
        clockValueTree.addListener(this);

        generator = std::default_random_engine(rd());

        distribution = std::uniform_real_distribution(0.2f, 0.8f);

        createWavetable();
    }

    //AudioProcessor
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        //juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2};

        juce::ignoreUnused(samplesPerBlock);

        clockSampleRate = sampleRate;

        interval_val = milliToSamples(BPMToMilli(120), sampleRate);

        counter_val = 0; //reset counter

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
            //auto totalNumInputChannels  = getTotalNumInputChannels();
            auto totalNumOutputChannels = getTotalNumOutputChannels();

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                if (++counter_val >= interval_val)
                {
                    counter_val = 0;
                    if (updateHasBeenSent) //better way of resetting this flag?
                    {
                        updateHasBeenSent = false;
                    }

                }
                if (counter_val < note_length)
                {
                    if (!updateHasBeenSent)
                    {
                        sendChangeMessage();
                        updateHasBeenSent = true;
                    }
                    for (int channel = 0; channel < totalNumOutputChannels; channel++)
                    {
                        auto* channelData = buffer.getWritePointer (channel);
                        //float randomFloat = distribution(generator);
                        auto levelSample = sineTone->getNextSample() * level;
                        channelData[sample] = levelSample;
                    }
                }
                else
                {
                    for (int channel = 0; channel < totalNumOutputChannels; channel++)
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

    void releaseResources() override
    {

    }

    void reset() override
    {

    }
    //

    //ValueTree Listener
    void handleAsyncUpdate() override
    {
        {
            int bpm = clockValueTree.getProperty(SP_ID::metronome_bpm);
            interval_val = milliToSamples(BPMToMilli(bpm), clockSampleRate);
        }
    }

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
        if (treeWhosePropertyHasChanged.getType() == SP_ID::METRONOME_BRANCH)
            if (property == SP_ID::metronome_bpm)
                triggerAsyncUpdate();
            if (property == SP_ID::metronome_grouping)
                std::cout << "bar grouping changed" << std::endl;
            if (property == SP_ID::metronome_gain)
                level = treeWhosePropertyHasChanged.getProperty(property);
    }

    //

    // CLock
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

    void setNumerator(int numerator) //passing by reference since these values won't be modified
    {
    numerator_val = numerator;
    }

    void setDenominator(int denominator)
    {
        denominator_val = denominator;
    }

    void setInterval(const int interval)
    {
        interval_val = interval;
    }

    void setBPM(int bpm)
    {
        bpm_val = bpm;
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

    int bpm_val;
    int numerator_val;
    int denominator_val;

    int interval_val;
    int counter_val;

    int note_length;

    double clockSampleRate;

    //wavetable
    const unsigned int tableSize = 1 << 7;
    std::atomic<float> level = 0.5f; //must be thread safe
    juce::AudioSampleBuffer sineTable;
    WavetableOscillator *sineTone{};

    std::random_device rd;
    std::default_random_engine generator;

    std::uniform_real_distribution<float> distribution;
};

