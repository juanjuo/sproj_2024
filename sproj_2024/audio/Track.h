//
// Created by Juan Diego on 1/17/25.
//

#pragma once
#include <Recorder.h>
#include <AudioPlayer.h>
#include <SPCommandManager.h>


class Track final : public SPAudioProcessor,
                    public juce::ApplicationCommandTarget
{
public:

    enum class ProcessorMode
    {
        recorder_Type = 1,
        player_Type
    };

    Track(juce::ValueTree valueTree, SPCommandManager& manager) : commandManager(manager)
    {
        juce::ignoreUnused(valueTree);
        commandManager.registerAllCommandsForTarget(this);
        commandManager.addTargetToCommandManager(this);
    }

    ~Track()
    {
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        if (currentType == ProcessorMode::recorder_Type)
            recorder.prepareToPlay(sampleRate, samplesPerBlock);

        if (currentType == ProcessorMode::player_Type)
            player.prepareToPlay(sampleRate, samplesPerBlock);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer) override
    {

        if (currentType == ProcessorMode::recorder_Type)
            recorder.processBlock(buffer, midiBuffer);

        if (currentType == ProcessorMode::player_Type)
            player.processBlock(buffer, midiBuffer);
    }

    void releaseResources() override
    {

        if (currentType == ProcessorMode::recorder_Type)
            recorder.releaseResources();

        if (currentType == ProcessorMode::player_Type)
            player.releaseResources();
    }

    void startRecording()
    {
        auto parentDir = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);

        lastRecording = parentDir.getNonexistentChildFile("Sproj_demo_recording", ".wav");

        recorder.startRecording(lastRecording);
    }

    void stopRecording()
    {
        recorder.stop();

        std::cout << "Recording ended" << std::endl;

        if (juce::FileInputStream inputStream(lastRecording); inputStream.openedOk())
            if (juce::FileOutputStream outputStream{lastRecording}; outputStream.openedOk())
                outputStream.flush();
                std::cout << "Outputed corrently" << std::endl;
    }

    void cropRecording()
    {
    }


    //ApplicationCommandTarget methods

    ApplicationCommandTarget *getNextCommandTarget() override
    {
        return nullptr;
    }

    void getAllCommands(juce::Array<juce::CommandID> &c) override
    {
        juce::Array<juce::CommandID> commands{
            SP_CommandID::record,
            SP_CommandID::play
        };
        c.addArray(commands);
    }

    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result) override
    {
        switch (commandID)
        {
            case SP_CommandID::record:
                result.setInfo("Record", "record and save in disk", "Audio", 0);
                result.setTicked(false);
                result.addDefaultKeypress('r', juce::ModifierKeys::commandModifier);
                break;
            case SP_CommandID::play:
                result.setInfo("Play", "play recently recorded file", "Audio", 0);
                result.setTicked(false);
                result.addDefaultKeypress('p', juce::ModifierKeys::commandModifier);
            default:
                break;
        }
    }

    bool perform(const InvocationInfo &info) override
    {
        switch (info.commandID)
        {
            case SP_CommandID::record:
                if (recorder.isRecording())
                {
                    stopRecording();
                    currentType = ProcessorMode::player_Type;
                } else
                {
                    currentType = ProcessorMode::recorder_Type;
                    startRecording();
                }
                break;
            case SP_CommandID::play:
                player.startOrStop();
                break;
            default:
                return false;
        }

        return true;
    }

private:
    ProcessorMode currentType = ProcessorMode::player_Type;

    Recorder recorder {};
    AudioPlayer player {};

    SPCommandManager &commandManager;
    juce::File lastRecording;
};
