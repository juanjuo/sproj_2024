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

    Scheduler(const juce::ValueTree& valueTree, SPCommandManager& cm, const juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& array) : valueTree(valueTree), commandManager(cm), trackArray(array)
    {

    }

    ~Scheduler()
    {

    }

    void update()
    {
        count--;
        if (count == -1)
            trigger();
    }

    void trigger()
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
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
    }


private:
    int count = 8;

    juce::ValueTree valueTree;
    SPCommandManager& commandManager;

    const juce::ReferenceCountedArray<juce::AudioProcessorGraph::Node>& trackArray;
};

