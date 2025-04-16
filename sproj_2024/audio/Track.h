//
// Created by Juan Diego on 1/17/25.
//

#pragma once
#include "Recorder.h"
#include "AudioPlayer.h"
#include <SPCommandManager.h>
#include "AudioClock.h"
#include <helpers.h>

/*  Track
 *
 *  Higher level class for handling recording and playback, as well as the scheduling of clips
 */
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

    // To hold Clip data
    struct ClipData
    {
        juce::String ID;
        int start;
        int end;
    };

    Track(const juce::ValueTree& tree, const juce::ValueTree& fdeck, SPCommandManager& manager, AudioClock* clock)
    : commandManager(manager), trackValueTree(tree), freeDeckValueTree(fdeck)
    {
        commandManager.registerAllCommandsForTarget(this);
        commandManager.addTargetToCommandManager(this);

        player.setAudioSource(juce::URL{lastRecording});

        trackValueTree.addListener(this);
        clock->addChangeListener(this);
    }

    // when a clip has been recorded, update it's 'ready to play' status
    void updateAllClipValueTrees(const juce::ValueTree& node) const
    {
        for (auto clip : trackValueTree)
        {
            if (clip.getProperty(SP_ID::U_ID) == node.getProperty(SP_ID::U_ID))
            {
                clip.setProperty(SP_ID::clip_ready_to_play, 1, nullptr);
            }
        }
    }

    ~Track() override
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

    void startRecording(const juce::File& file)
    {
        lastRecording = file;
        recorder.startRecording(file);
    }

    void stopRecording()
    {
        recorder.stop();

        if (juce::FileInputStream inputStream(lastRecording); inputStream.openedOk())
            if (juce::FileOutputStream outputStream{lastRecording}; outputStream.openedOk())
                outputStream.flush();
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        recorder.prepareToPlay(sampleRate, samplesPerBlock);
        player.prepareToPlay(sampleRate, samplesPerBlock);
    }

    //This method is the one in charged of changing the functionality of the track, by bypassing the buffer
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
        const juce::Array<juce::CommandID> commands{
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
                //startRecording();
            }
            break;
        case SP_CommandID::play:
            //player.startOrStop();
            break;
        default:
            return false;
        }

        return true;
    }

    //ValueTreeListener methods
    //Update Track Gain
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override
    {
        if (trackValueTree == treeWhosePropertyHasChanged)
            if (property == SP_ID::track_gain)
                player.setGain(treeWhosePropertyHasChanged.getProperty(property));
    }

    // Parse clip data from ValueTree into clipData struct
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        if (parentTree.getType() == SP_ID::TRACK)
            if (childWhichHasBeenAdded.getType() == SP_ID::CLIP)
            {
                auto id = childWhichHasBeenAdded.getProperty(SP_ID::U_ID).toString();
                auto startValue = childWhichHasBeenAdded.getProperty(SP_ID::clip_start_value);
                auto endValue = childWhichHasBeenAdded.getProperty(SP_ID::clip_end_value);
                if (childWhichHasBeenAdded.getProperty(SP_ID::clip_filepath) == " ")
                {
                    //set up a new file for clip
                    auto parentDir = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
                    auto fileName = createNewFileName();
                    auto filePath = parentDir.getNonexistentChildFile(fileName, ".wav");
                    childWhichHasBeenAdded.setProperty(SP_ID::clip_filepath, filePath.getFullPathName(), nullptr);
                    auto freeDeckClip = freeDeckValueTree.getChildWithProperty(SP_ID::U_ID, id);
                    freeDeckClip.setProperty(SP_ID::clip_filepath, filePath.getFullPathName(), nullptr);
                }
                clipData.push_back({id, startValue, endValue});
                // std::cout << "scheduler hashmap updated (clip added)" << std::endl;
                // for (const auto& data : clipData)
                // {
                //     std::cout << "Key: " << data.ID << ", start value: " << data.start << ", end value: "
                //         << data.end << std::endl;
                // }
            }
    }

    //Change Listener Methods
    /* Scheduler
     *
     *  This method is called by the clock every beat, and searches through the clipData vector for clips ready to be recorded/played
     */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        juce::ignoreUnused(source);
        if (!clipData.empty())
        {
            for (auto data : clipData)
            {
                auto clip = trackValueTree.getChildWithProperty(SP_ID::U_ID, data.ID);
                if (data.start == currentBeat)
                {
                    if (clip.getProperty(SP_ID::clip_ready_to_play).equals(0)) //better way of checking if a filepath is valid?
                    {
                        currentType = ProcessorMode::recorder_Type;
                        juce::String file = clip.getProperty(SP_ID::clip_filepath).toString();
                        startRecording(file);
                        //clip.setProperty(SP_ID::clip_filepath, lastRecording.getFullPathName(), nullptr);
                    }
                    else if (clip.getProperty(SP_ID::clip_ready_to_play).equals(1))
                    {
                        currentType = ProcessorMode::player_Type;
                        juce::String file = clip.getProperty(SP_ID::clip_filepath).toString();
                        startPlaying(file);
                    }
                }
                if (data.end == currentBeat)
                {
                    if (currentType == ProcessorMode::recorder_Type)
                    {
                        stopRecording();
                        updateAllClipValueTrees(clip);
                        freeDeckValueTree.getChildWithProperty(SP_ID::U_ID, clip.getProperty(SP_ID::U_ID)).setProperty(SP_ID::clip_ready_to_play, 1, nullptr);

                    }
                    else if (currentType == ProcessorMode::player_Type)
                    {
                        stopPlaying();
                    }
                }
            }
            std::erase_if(clipData, [this](const auto& element) { return element.end == currentBeat; });
            //for erasing the elements that hit 0. this is really slow!
        }

        currentBeat++;
        if (currentBeat == 160) //For further implementations: do this during runtime
        {

            currentBeat = 0;
            //std::cout << currentBeat << std::endl;
        }
    }

private:
    ProcessorMode currentType = ProcessorMode::player_Type;

    SPCommandManager& commandManager;

    juce::File lastRecording;

    Recorder recorder{};
    AudioPlayer player{};

    float gain = player.getGain();

    int numFiles = 0;

    juce::ValueTree trackValueTree;
    juce:: ValueTree freeDeckValueTree;

    std::vector<ClipData> clipData; //to hold all clip values and ID

    int currentBeat = 0;

    juce::String createNewFileName()
    {
        std::ostringstream oss;
        numFiles++;
        oss << "Sproj2024_file_" << std::setw(3) << std::setfill('0') << numFiles;
        return juce::String{oss.str()};
    }
};
