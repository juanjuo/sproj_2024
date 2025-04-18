#include <MainAudio.h>
#include "MainWindow.h"
#include "helpers.h"
#include <SPCommandManager.h>
#include "focusrite/e2e/TestCentre.h"

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
 * - implement looping stage (including playhead, drag and drop, a grid that links to the metrono)
 * - Project Management (load, save, name)
 * - Use Debuggin for screen ((Put JUCE_ENABLE_REPAINT_DEBUGGING=1 in
 *   target_compile_definitions in your CMakeLists.txt))
 * - Use JUCE_LIVE_CONSTANT (lets you manipulate any value live to fine-tune them)
 *
 * AUDIO:
 * - Device Manager
 * - Recording - using device manager and writing recorded buffers to disk
 * - Playback - using recorded buffers from disk and using device manager
 * - MIDI
 * - Playback commands (start, stop)
 * - Moving Playhead
 * - Improve Metronome, make it identify beats and produce different tones
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
    GuiAppApplication() = default;

    // We inject these as compile definitions from the CMakeLists.txt
    // If you've enabled the juce header with `juce_generate_juce_header(<thisTarget>)`
    // you could `#audio <JuceHeader.h>` and use `ProjectInfo::projectName` etc. instead.
    const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override { return false; }

    //==============================================================================
    void initialise(const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code...
        juce::ignoreUnused(commandLine);

        valueTree.appendChild(juce::ValueTree{SP_ID::METRONOME_BRANCH}, nullptr);

        valueTree.appendChild(juce::ValueTree{SP_ID::TRACK_BRANCH}, nullptr);

        valueTree.appendChild(juce::ValueTree{SP_ID::FREEDECK_BRANCH}, nullptr);

        mainAudio = std::make_unique<MainAudio>(valueTree, *commandManager, *deviceManager);

        mainComponent = std::make_unique<MainComponent>(valueTree, *commandManager, *deviceManager);

        mainWindow = std::make_unique<MainWindow>(getApplicationName(), mainComponent.get());

        commandManager->registerAllCommandsForTarget(this);
        commandManager->addTargetToCommandManager(this);

        //std::cerr << commandManager->getNumCommands() << std::endl;

        //std::cerr << commandManager->invokeDirectly(SP_CommandID::print, true) << std::endl;

        testCentre = focusrite::e2e::TestCentre::create();
    }

    void shutdown() override
    {
        // Add your application's shutdown code here...
        mainComponent = nullptr;
        mainWindow = nullptr;
        commandManager = nullptr;
        mainAudio = nullptr;
        deviceManager = nullptr;

        testCentre.reset();
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    //ApplicationCommandTarget methods

    juce::ApplicationCommandTarget* getNextCommandTarget() override
    {
        return nullptr;
    }

    void getAllCommands(juce::Array<juce::CommandID>& c) override
    {
        juce::Array<juce::CommandID> commands{
            SP_CommandID::startOrStopProcessing
        };
        c.addArray(commands);
    }

    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
    {
        switch (commandID)
        {
        case SP_CommandID::startOrStopProcessing:
            result.setInfo("start or stop processing", "starts or stops playback", "Audio", 0);
            result.setTicked(false);
            result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers);
            break;
        default:
            break;
        }
    }

    bool perform(const InvocationInfo& info) override
    {
        switch (info.commandID)
        {
        case SP_CommandID::startOrStopProcessing:
            mainAudio->pauseOrResumeProcessing();
            mainComponent->startOrStopAnimation();
            break;
        default:
            return false;
        }
        return true;
    }

private:
    std::unique_ptr<juce::AudioDeviceManager> deviceManager = std::make_unique<juce::AudioDeviceManager>();
    std::unique_ptr<MainAudio> mainAudio;
    std::unique_ptr<SPCommandManager> commandManager = std::make_unique<SPCommandManager>();
    std::unique_ptr<MainComponent> mainComponent;
    std::unique_ptr<MainWindow> mainWindow;
    juce::ValueTree valueTree {SP_ID::MAIN_BRANCH};

    //End-to-End Testing
    std::unique_ptr<focusrite::e2e::TestCentre> testCentre;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(GuiAppApplication)
