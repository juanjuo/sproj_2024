//
// Created by Juan Diego on 1/8/25.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <helpers.h>
#include <SPCommandManager.h>

class MenuComponent final : public juce::Component,
                            public juce::MenuBarModel,
                            public juce::ApplicationCommandTarget
{
public:
    explicit MenuComponent(SPCommandManager& manager)
        : commandManager(manager)
    {
        setApplicationCommandManagerToWatch (&commandManager); //for MenuBarModel
        commandManager.registerAllCommandsForTarget (this); //REGISTERING COMMANDS FOR THIS TARGET
        commandManager.setFirstCommandTarget (this);

        addKeyListener (commandManager.getKeyMappings()); //ADD KEY MAPPINGS

        setWantsKeyboardFocus (true);
        #if JUCE_MAC
        setMacMainMenu(this);
        #endif

        commandManager.addTargetToCommandManager(this);
    }

    ~MenuComponent() override
    {
       #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
       #endif
    }

    //MenuBar Methods
    juce::StringArray getMenuBarNames() override
    {
        return {"Extra"};
    }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String & /*menuName*/) override
    {
        juce::PopupMenu menu;

        if (menuIndex == 0)
        {
            #if JUCE_MAC
            menu.addCommandItem(&commandManager, SP_CommandID::print);
            menu.addCommandItem(&commandManager, SP_CommandID::show_audio_device_menu);
            menu.addCommandItem(&commandManager, SP_CommandID::record);
            menu.addCommandItem(&commandManager, SP_CommandID::play);
            menu.addCommandItem(&commandManager, SP_CommandID::startOrStopProcessing);
            menu.addCommandItem(&commandManager, SP_CommandID::createNewTrack);
            menu.addCommandItem(&commandManager, SP_CommandID::createNewDummyClip);
            #endif
        }

        return menu;
    }

    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override
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
            SP_CommandID::print
        };
        c.addArray(commands);
    }

    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result) override
    {
        switch (commandID)
        {
            case SP_CommandID::print:
                result.setInfo("Print", "Prints something to the console", "Print", 0);
                result.setTicked(false);
                //result.setActive(true);
                result.addDefaultKeypress('p', juce::ModifierKeys::shiftModifier);
                break;
            default:
                break;
        }
    }

    bool perform(const InvocationInfo &info) override
    {
        switch (info.commandID)
        {
            case SP_CommandID::print:
                std::cerr << "PRINTING TO THE SCREEN" << std::endl;
                break;
            default:
                return false;
        }

        return true;
    }

private:
    SPCommandManager& commandManager;
};

