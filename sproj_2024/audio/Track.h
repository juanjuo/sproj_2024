//
// Created by Juan Diego on 1/17/25.
//

/* TODO:
 * maybe save all File pointers into an array and give them directly to the audioPlayer?
 * have to change AudioTransportSource, since this class is meant to be used Asynchronously
 *
 */

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
        //player.setLooping(true); find a way to trigger looping from Track class (because of Command Target)

        player.setAudioSource(juce::URL {lastRecording});
    }

    ~Track()
    {
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override //is there a better way to do this?
    {
        //if (currentType == ProcessorMode::recorder_Type)
            recorder.prepareToPlay(sampleRate, samplesPerBlock);

        //if (currentType == ProcessorMode::player_Type)
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

        lastRecording = parentDir.getNonexistentChildFile(createNewFileName(), ".wav");

        recorder.startRecording(lastRecording);
        std::cout << lastRecording.getFileName() << std::endl;
    }

    void stopRecording()
    {
        recorder.stop();

        std::cout << "Recording ended" << std::endl;

        if (juce::FileInputStream inputStream(lastRecording); inputStream.openedOk())
            if (juce::FileOutputStream outputStream{lastRecording}; outputStream.openedOk())
                outputStream.flush();
                std::cout << "Outputed corrently" << std::endl;
                player.setAudioSource(juce::URL {lastRecording});
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
                    stopRecording();
                    currentType = ProcessorMode::player_Type;
                } else
                {
                    currentType = ProcessorMode::recorder_Type;
                    std::cout << "recording mode" << std::endl;
                    startRecording();
                }
                break;
            case SP_CommandID::play:
                player.startOrStop();
                //std::cout << player.isLooping() << std::endl;
                break;
            default:
                return false;
        }

        return true;
    }

private:
    ProcessorMode currentType = ProcessorMode::player_Type;

    //juce::URL currentAudioFile {juce::File("/Users/juan/Desktop/Sunny.mp3")};

    SPCommandManager& commandManager;
    juce::File lastRecording {juce::File("/Users/juan/Desktop/Sunny.mp3")};

    Recorder recorder {};
    AudioPlayer player {};

    int numFiles = 0; //make this global/static

    juce::String createNewFileName() //maybe make this static as well??
    {
        std::ostringstream oss;
        numFiles++;
        oss << "Sproj2024_file_" << std::setw(3) << std::setfill('0') << numFiles;
        return juce::String {oss.str()};
    }
};
