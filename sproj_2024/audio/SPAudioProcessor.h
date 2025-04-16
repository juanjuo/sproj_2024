//
// Created by Juan Diego on 10/23/24.
//
#pragma once

#include <../../cmake-build-debug/lib/juce/modules/juce_audio_processors/juce_audio_processors.h>
#include <../../cmake-build-debug/lib/juce/modules/juce_dsp/juce_dsp.h>

/*  SPAudioProcessor
 *
 *  Dedicated AudioProcessor for all audio processing classes (Track, Clock, AudioPlayer, Recorder)
 */
class SPAudioProcessor : public juce::AudioProcessor,
                         public juce::AsyncUpdater
{
public:
    SPAudioProcessor();
    ~SPAudioProcessor() override;

    void pauseOrResumeProcessing();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    using juce::AudioProcessor::getName;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //FROM ASYNC UPDATER
    void handleAsyncUpdate() override;

protected:
    bool isPaused = true;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SPAudioProcessor)
};
