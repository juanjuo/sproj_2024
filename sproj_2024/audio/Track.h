//
// Created by Juan Diego on 1/17/25.
//

#pragma once
#include <Record.h>
#include <SPCommandManager.h>


class Track final : public SPAudioProcessor,
                    public juce::ApplicationCommandTarget
{
public:
    Track(juce::ValueTree valueTree, SPCommandManager& manager) : commandManager(manager)
    {
        juce::ignoreUnused(valueTree);
        commandManager.registerAllCommandsForTarget(this);
        commandManager.addTargetToCommandManager(this);
        std::cout << "Track created" << std::endl;
    }

    ~Track()
    {
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        recorder.prepareToPlay(sampleRate, samplesPerBlock);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer) override
    {
        recorder.processBlock(buffer, midiBuffer);
    }

    void releaseResources() override
    {
        recorder.releaseResources();
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
            SP_CommandID::record
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
            //result.setActive(true);
                result.addDefaultKeypress('r', juce::ModifierKeys::commandModifier);
            //commandManager.commandStatusChanged();
                break;
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
                    std::cout << recorder.isRecording() << std::endl;
                    stopRecording();
                } else
                {
                    std::cout << recorder.isRecording() << std::endl;
                    startRecording();
                    std::cout << recorder.isRecording() << std::endl;
                }
                break;
            default:
                return false;
        }

        return true;
    }

private:
    Record recorder {};
    SPCommandManager &commandManager;
    juce::File lastRecording;
};
