//
// Created by Juan Diego on 9/30/24.
//
#pragma once

#include <SPAudioProcessor.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <Identifiers.h>
#include <random>

class Clock : public SPAudioProcessor,
              public juce::ValueTree::Listener
{
public:

    Clock(juce::ValueTree v);

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void releaseResources() override;

    void reset() override;

    void handleAsyncUpdate() override;

    void setNumerator(int numerator); //passing by reference since these values won't be modified

    void setDenominator(int denominator);

    void setInterval(float interval);

    void setBPM(int bpm);

    float BPMToMilli(int bpm);

    int milliToSamples(int milliseconds, double sampleRate);

    float milliToBPM(float milliseconds);

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

private:

    juce::ValueTree clockValueTree;

    juce::dsp::Oscillator<float> osc;

    int bpm_val;
    int numerator_val;
    int denominator_val;

    int interval_val;
    int counter_val;

    int note_length;

    double clockSampleRate;


    std::random_device rd;
    std::default_random_engine generator;

    std::uniform_real_distribution<float> distribution;

};

