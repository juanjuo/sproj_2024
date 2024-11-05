#include <Clock.h>
#include <MainAudio.h>

#include "MainComponent.h"
#include "MainWindow.h"
#include "Identifiers.h"

//==============================================================================
class GuiAppApplication final : public juce::JUCEApplication
{
public:
    //==============================================================================
    GuiAppApplication() {}

    // We inject these as compile definitions from the CMakeLists.txt
    // If you've enabled the juce header with `juce_generate_juce_header(<thisTarget>)`
    // you could `#audio <JuceHeader.h>` and use `ProjectInfo::projectName` etc. instead.
    const juce::String getApplicationName() override       { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override    { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override             { return false; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        juce::ignoreUnused (commandLine);

        mainAudio = std::make_unique<MainAudio>();

        mainWindow.reset (new MainWindow (getApplicationName(), valueTree));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        //clock = nullptr;
        mainAudio = nullptr;

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

private:
    juce::ValueTree valueTree {SP_ID::MAIN_BRANCH};
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MainAudio> mainAudio;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (GuiAppApplication)
