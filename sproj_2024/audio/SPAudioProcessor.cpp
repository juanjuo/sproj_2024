//
// Created by Juan Diego on 10/23/24.
//

#include "SPAudioProcessor.h"

SPAudioProcessor::SPAudioProcessor()
     : juce::AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

SPAudioProcessor::~SPAudioProcessor() = default;

void SPAudioProcessor::pauseOrResumeProcessing()
{
    if (isPaused) isPaused = false; else isPaused = true;
}

const juce::String SPAudioProcessor::getName() const
{
    return "Sproj2024";
}

bool SPAudioProcessor::acceptsMidi() const
{
   return false;
}

bool SPAudioProcessor::producesMidi() const
{
   return false;
}

bool SPAudioProcessor::isMidiEffect() const
{
   return false;
}

double SPAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SPAudioProcessor::getNumPrograms()
{
    return 1;
}

int SPAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SPAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SPAudioProcessor::getProgramName (int index)
{
    return "Sproj2024";
}

void SPAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void SPAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void SPAudioProcessor::releaseResources()
{

}

void SPAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused (midiMessages);
}

//==============================================================================
juce::AudioProcessorEditor* SPAudioProcessor::createEditor()
{
    return nullptr;
}

bool SPAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

//==============================================================================
void SPAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void SPAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SPAudioProcessor();
}

//FROM ASYNC UPDATER

void SPAudioProcessor::handleAsyncUpdate()
{
}


