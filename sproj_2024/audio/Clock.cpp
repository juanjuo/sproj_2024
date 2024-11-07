//
// Created by Juan Diego on 9/30/24.
//

#include "Clock.h"

Clock::Clock(juce::ValueTree v) : interval_val(5000), counter_val(0), note_length(2000), clockValueTree(v), clockSampleRate(44100)
{
    clockValueTree.addListener(this);

    generator = std::default_random_engine(rd());

    distribution = std::uniform_real_distribution<float>(0.2, 0.8);

    osc.setFrequency(440.0f);
    osc.initialise([] (float x)
        {
        return std::sin (x);
        });
}

void Clock::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2};
    osc.prepare(spec);

    clockSampleRate = sampleRate;

    interval_val = milliToSamples(BPMToMilli(120), sampleRate);

    counter_val = 0; //reset counter
}

void Clock::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //clean extra output channels
    // for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    // {
    //     buffer.clear (i, 0, buffer.getNumSamples());
    // }

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (++counter_val >= interval_val) counter_val = 0;
        if (counter_val < note_length)
        {
            for (int channel = 0; channel < totalNumOutputChannels; channel++)
            {
                auto* channelData = buffer.getWritePointer (channel);
                float randomFloat = distribution(generator);
                channelData[sample] = randomFloat;

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

void Clock::releaseResources()
{
    //do something??
}


void Clock::reset()
{
    osc.reset();
}

void Clock::handleAsyncUpdate()
{
    int bpm = clockValueTree.getProperty(SP_ID::bpm);
    interval_val = milliToSamples(BPMToMilli(bpm), clockSampleRate);
    std::cout << "from clock class" << std::endl;
}


void Clock::setNumerator(int numerator)
{
    numerator_val = numerator;
}

void Clock::setDenominator(int denominator)
{
    denominator_val = denominator;
}

void Clock::setInterval(float interval)
{
    interval_val = interval;
}

void Clock::setBPM(int bpm)
{
    bpm_val = bpm;
}

float Clock::BPMToMilli(int bpm)
{
    if (bpm <= 0) {
        std::cerr << "BPM must be a positive integer." << std::endl;
        return -1; // Indicate an error
    }
    return 60000.0f / bpm; // Convert BPM to milliseconds
}

float Clock::milliToBPM(float milliseconds)
{
    if (milliseconds <= 0) {
        std::cerr << "Milliseconds must be a positive value." << std::endl;
        return -1; // Indicate an error
    }
    return 60000.0f / milliseconds; // Convert milliseconds to BPM
}

int Clock::milliToSamples(int milliseconds, double sampleRate)
{
    double samples = sampleRate * (milliseconds / 1000.0);
    return static_cast<int> (samples);
}

void Clock::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    triggerAsyncUpdate();
}








