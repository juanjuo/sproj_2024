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
#include <AudioClock.h>

class Track final : public SPAudioProcessor,
                    public juce::ApplicationCommandTarget,
                    public juce::ValueTree::Listener,
                    public juce::ChangeListener
{
public:
    enum class ProcessorMode
    {
        recorder_Type = 1,
        player_Type
    };

    struct ClipData
    {
        juce::String ID;
        int start;
        int end;
        bool finished = false;
    };

    Track(juce::ValueTree& tree, SPCommandManager& manager, AudioClock* clock) : commandManager(manager),
        valueTree(tree)
    {
        commandManager.registerAllCommandsForTarget(this);
        commandManager.addTargetToCommandManager(this);
        //player.setLooping(true); find a way to trigger looping from Track class (because of Command Target)

        player.setAudioSource(juce::URL{lastRecording});

        valueTree.addListener(this);
        clock->addChangeListener(this);
    }

    ~Track()
    {
    }

    void startPlaying(const juce::File& file)
    {
        if (file.exists())
        {
            player.setAudioSource(juce::URL{file});
            player.startPlayer();
        }
    }

    void stopPlaying()
    {
        player.stopPlayer();
    }


    void startRecording() //SET VALUE TREE CLIP TO THIS VALUE
    {
        currentType = ProcessorMode::recorder_Type;
        auto parentDir = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);

        auto fileName = createNewFileName();

        lastRecording = parentDir.getNonexistentChildFile(fileName, ".wav");

        recorder.startRecording(lastRecording);
        //std::cout << lastRecording.getFileName() << std::endl;
    }

    void stopRecording()
    {
        recorder.stop();

        //std::cout << "Recording ended" << std::endl;

        if (juce::FileInputStream inputStream(lastRecording); inputStream.openedOk())
            if (juce::FileOutputStream outputStream{lastRecording}; outputStream.openedOk())
                outputStream.flush();
        currentType = ProcessorMode::player_Type;
        //std::cout << "Outputed corrently" << std::endl;
        //player.setAudioSource(juce::URL {lastRecording});
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
        if (!isPaused)
        {
            if (currentType == ProcessorMode::recorder_Type)
                recorder.processBlock(buffer, midiBuffer);

            if (currentType == ProcessorMode::player_Type)
                player.processBlock(buffer, midiBuffer);
        }
        else
        {
            buffer.clear();
        }
    }

    void releaseResources() override
    {
        if (currentType == ProcessorMode::recorder_Type)
            recorder.releaseResources();

        if (currentType == ProcessorMode::player_Type)
            player.releaseResources();
    }

    //ApplicationCommandTarget methods

    ApplicationCommandTarget* getNextCommandTarget() override
    {
        return nullptr;
    }

    void getAllCommands(juce::Array<juce::CommandID>& c) override
    {
        juce::Array<juce::CommandID> commands{
            SP_CommandID::record,
            SP_CommandID::play
        };
        c.addArray(commands);
    }

    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
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

    bool perform(const InvocationInfo& info) override
    {
        switch (info.commandID)
        {
        case SP_CommandID::record:
            if (recorder.isRecording())
            {
                stopRecording();
                currentType = ProcessorMode::player_Type;
            }
            else
            {
                currentType = ProcessorMode::recorder_Type;
                std::cout << "recording mode" << std::endl;
                startRecording();
            }
            break;
        case SP_CommandID::play:
        //player.startOrStop();
        //std::cout << player.isLooping() << std::endl;
            break;
        default:
            return false;
        }

        return true;
    }

    //ValueTreeListener methods

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        if (valueTree == treeWhosePropertyHasChanged)
            if (property == SP_ID::track_gain)
                player.setGain(treeWhosePropertyHasChanged.getProperty(property));
            else std::cout << "audio track property doesn't match" << std::endl;
    }

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        if (parentTree.getType() == SP_ID::TRACK)
            if (childWhichHasBeenAdded.getType() == SP_ID::CLIP)
            {
                auto id = childWhichHasBeenAdded.getProperty(SP_ID::U_ID).toString();
                auto startValue = childWhichHasBeenAdded.getProperty(SP_ID::clip_start_value);
                auto endValue = childWhichHasBeenAdded.getProperty(SP_ID::clip_end_value);
                clipData.push_back({id, startValue, endValue});
                std::cout << "scheduler hashmap updated (clip added)" << std::endl;
                for (const auto& data : clipData)
                {
                    std::cout << "Key: " << data.ID << ", start value: " << data.start << ", end value: "
                        << data.end << std::endl;
                }
            }
    }

    //Change Listener Methods
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (currentBeat < 160)//do this during runtime
        {
            std::cout << currentBeat << std::endl;
            currentBeat++;
        }
        else
            currentBeat = 0;

        if (!clipData.empty())
            for (auto data : clipData)
            {
                if (data.start == currentBeat)
                {
                    auto clip = valueTree.getChildWithProperty(SP_ID::U_ID, data.ID);
                    if (clip.getProperty(SP_ID::clip_filepath) == " ") //better way of checking if a filepath is valid?
                    {
                        startRecording();
                        clip.setProperty(SP_ID::clip_filepath, lastRecording.getFullPathName(), nullptr);
                        currentType = ProcessorMode::recorder_Type;
                    }
                    else if (currentType == ProcessorMode::player_Type)
                    {
                        juce::String file = clip.getProperty(SP_ID::clip_filepath);
                        startPlaying(file);
                    }
                }

                if (data.end == currentBeat)
                {
                    if (currentType == ProcessorMode::recorder_Type)
                    {
                        stopRecording();
                        currentType = ProcessorMode::player_Type;
                        data.finished = true;
                    }
                    else if (currentType == ProcessorMode::player_Type)
                    {
                        stopPlaying();
                        data.finished = true;
                    }
                }
            }
    }

private:
    ProcessorMode currentType = ProcessorMode::player_Type;

    SPCommandManager& commandManager;
    //juce::File lastRecording{juce::File("/Users/juan/Desktop/Sunny2.wav")};

    juce::File lastRecording;

    Recorder recorder{};
    AudioPlayer player{};

    float gain = player.getGain();

    int numFiles = 0; //make this global/static

    juce::ValueTree valueTree;

    std::vector<ClipData> clipData; //to hold all clip values and ID

    int currentBeat = 0;

    juce::String createNewFileName() //maybe make this static as well??
    {
        std::ostringstream oss;
        numFiles++;
        oss << "Sproj2024_file_" << std::setw(3) << std::setfill('0') << numFiles;
        return juce::String{oss.str()};
    }
};
