//
// Created by Juan Diego on 1/13/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SPCommandManager final : public juce::ApplicationCommandManager
{
public:
    SPCommandManager()
    {
    }

    ~SPCommandManager() override = default;

    void addTargetToCommandManager(juce::ApplicationCommandTarget* target)
    {
        targets.add(target);
    }

    //Looks for the target that corresponds to this commandID
    juce::ApplicationCommandTarget* getFirstCommandTarget(juce::CommandID commandID) override
    {
        if (targets.size() > 0)
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
        }
        else
        {
            std::cout << "Command not found" << std::endl;
        }
        return nullptr;
    }

private:
    juce::Array<juce::ApplicationCommandTarget*> targets;
};

