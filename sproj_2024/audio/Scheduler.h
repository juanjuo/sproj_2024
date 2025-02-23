//
// Created by Juan Diego on 1/19/25.
//

#pragma once
#include <juce_data_structures/juce_data_structures.h>
#include <SPCommandManager.h>
#include <Track.h>

class Scheduler final : public juce::ValueTree::Listener
{
public:

    Scheduler(juce::ValueTree& v, SPCommandManager& cm, const juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& array)
        : valueTree(v.getChildWithName(SP_ID::TRACK_BRANCH)), commandManager(cm), trackArray(array)
    {
        valueTree.addListener(this);
    }

    ~Scheduler()
    {

    }

    void update() //is this thread safe? is this real-time safe?
    {
        if (hashMap.size() > 0)
        {
            std::erase_if(hashMap, [](const auto& element) { return element.second == 0;}); //for erasing the elements that hit 0. this is really slow!
            for (auto& startPoint : hashMap)
            {
                startPoint.second -= 1;
                if (startPoint.second == 0)
                {
                    trigger();
                }
            }
        }
    }

    void trigger() //is this thread safe? is this real-time safe?
    {
        auto node = trackArray.getLast();
        auto track = dynamic_cast<Track*>(node->getProcessor());
        if (track != nullptr)
            track->trackStartOrStop();
    }

    void handleAsyncUpdate()
    {
    }

    //ValueTree
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override
    {
        if (treeWhosePropertyHasChanged.getType() == SP_ID::CLIP)
            if (property == SP_ID::clip_start_value)
                hashMap.emplace(treeWhosePropertyHasChanged.getProperty(SP_ID::U_ID).toString(), treeWhosePropertyHasChanged.getProperty(SP_ID::clip_start_value));
            else std::cout << "scheduler property doesn't match" << std::endl;
        else std::cout << "scheduler tree doesn't match" << std::endl;
    }

    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override
    {
        if (childWhichHasBeenAdded.getType() == SP_ID::CLIP)
                hashMap.emplace(childWhichHasBeenAdded.getProperty(SP_ID::U_ID).toString(), childWhichHasBeenAdded.getProperty(SP_ID::clip_start_value));
        else std::cout << "scheduler clip type doesn't match" << std::endl;
    }

private:
    //int count = 8;

    juce::ValueTree valueTree;
    SPCommandManager& commandManager;

    std::unordered_map<juce::String, int> hashMap; //to hold all clip_start_value

    // juce::HashMap<juce::var, int> hashMap;
    // juce::HashMap<juce::var, int>::Iterator i {hashMap}; //to hold all clip_start_value

    const juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& trackArray; //hold all tracks
};

/*using:
 *
 * for (datatype& variableName : container)
 * {
 * }
 *
 */

