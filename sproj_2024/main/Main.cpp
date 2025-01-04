#include <MainAudio.h>
#include "MainWindow.h"
#include "Identifiers.h"



/* TODO:
 *
 * TESTING:
 * - set up testing
 * - test audio classes (test for real time constrains in the audio thread)
 * - test gui (test for user response, how things are supposed to react when interacted with GUI)
 * - think about unit test and end-to-end testing?
 *
 * GUI:
 * - expand metronome, make number boxes behave correctly
 * - add Track Tree Structure
 * - implement looping stage??? (including playhead, drag and drop, a grid that links to the metrono)
 * - Project Management (load, save, name)
 * - LOOK INTO JIVE!!
 * - Use Debuggin for screen ((Put JUCE_ENABLE_REPAINT_DEBUGGING=1 in
 *   target_compile_definitions in your CMakeLists.txt))
 * - Use JUCE_LIVE_CONSTANT (lets you manipulate any value live to fine-tune them)
 *
 * AUDIO:
 * - Record
 * - MIDI
 * - Playback commands (start, stop)
 * - Moving Playhead
 * -
 *
 * LOOPER:
 * - Quantizing Commands ?? linked to the metronome
 * - Looping Recorded audio
 * - Scheduler?
 *
 */

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

        mainAudio = std::make_unique<MainAudio>(valueTree);

        mainWindow.reset (new MainWindow (getApplicationName(), valueTree));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
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
