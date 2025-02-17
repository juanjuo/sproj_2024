//
// Created by Juan Diego on 1/19/25.
//

#pragma once
#include <juce_data_structures/juce_data_structures.h>

class Scheduler final : public juce::ValueTree::Listener
{
public:

    Scheduler(const juce::ValueTree& valueTree) : valueTree(valueTree)
    {

    }

    ~Scheduler()
    {

    }

    void update()
    {
        count--;
    }

    void trigger()
    {

    }


    void handleAsyncUpdate()
    {
    }

    //ValueTree
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
    }


private:
    int count = 0;

    juce::ValueTree valueTree;
};

