//
// Created by Juan Diego on 1/13/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <Identifiers.h>

class SPCommandManager : public juce::ApplicationCommandManager
{
public:
    SPCommandManager() : juce::ApplicationCommandManager(), targets()
    {
    }

    ~SPCommandManager() = default;

    void addTargetToCommandManager(juce::ApplicationCommandTarget* target)
    {
        targets.add(target);
    }

    //Looks for the target that corresponds to this commandID
    juce::ApplicationCommandTarget* getFirstCommandTarget (juce::CommandID commandID) override
    {
        for (int i = 0; i < targets.size(); i++)
        {
            juce::Array<juce::CommandID> targetCommands;
             targets[i]->getAllCommands(targetCommands);
            if (targetCommands.contains(commandID))
            {
                return targets[i];
            }
        }

        std::cout << "Command not found" << std::endl;
        return nullptr;
    }

private:
    juce::Array<juce::ApplicationCommandTarget*> targets; //owned array?


};

