//
// Created by Juan Diego on 1/8/25.
//

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <Identifiers.h>

class MenuComponent final : public juce::Component,
                            public juce::MenuBarModel,
                            public juce::ApplicationCommandTarget
{
public:
    MenuComponent()
    {
        menuBar.reset (new juce::MenuBarComponent (this));
        addAndMakeVisible (menuBar.get());
        setApplicationCommandManagerToWatch (&commandManager); //for MenuBarModel
        commandManager.registerAllCommandsForTarget (this); //3 REGISTERING COMMANDS FOR THIS TARGET

        // this ensures that commands invoked on the DemoRunner application are correctly
        // forwarded to this demo
        commandManager.setFirstCommandTarget (this);

        // this lets the command manager use keypresses that arrive in our window to send out commands
        addKeyListener (commandManager.getKeyMappings()); //4 ADD KEY MAPPINGS

        // addChildComponent (menuHeader);
        // addAndMakeVisible (outerCommandTarget);
        // addAndMakeVisible (sidePanel);

        setWantsKeyboardFocus (true);
        #if JUCE_MAC
        setMacMainMenu(this);
        #endif

        setSize (500, 500);
    }

    ~MenuComponent() override
    {
       #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
       #endif

        commandManager.setFirstCommandTarget (nullptr);
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
            //menu.addCommandItem(&commandManager, CommandIDs::menuPositionInsideWindow);
            #if JUCE_MAC
            menu.addCommandItem(&commandManager, SP_CommandID::print);
            #endif
        }

        return menu;
    }

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override
    {
    }

    //ApplicationCommandTarget methods
    ApplicationCommandTarget *getNextCommandTarget() override
    {
        return findFirstTargetParentComponent();
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
    juce::ApplicationCommandManager commandManager;
    std::unique_ptr<juce::MenuBarComponent> menuBar;
};
