//
// Created by Juan Diego on 1/13/25.
//
#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <SPCommandManager.h>

//==============================================================================
class DeviceSelectionMenu final : public juce::DocumentWindow,
                                  public juce::ChangeListener,
                                  public juce::ApplicationCommandTarget
{
public:
    explicit DeviceSelectionMenu(juce::AudioDeviceManager& manager, SPCommandManager& command)
    : DocumentWindow("Audio Device Settings", juce::Colour::fromRGB(150, 150, 150), DocumentWindow::closeButton, false), commandManager(command), audioDeviceManager(manager),
          innerComponent(new InnerComponent(manager))
    {
        audioDeviceManager.addChangeListener(this);

        setUsingNativeTitleBar(true);
        setContentOwned(innerComponent, true);
        setResizable (false, false);
        centreWithSize (getWidth(), getHeight());
        setFullScreen(false);

        commandManager.registerAllCommandsForTarget(this);
        commandManager.addTargetToCommandManager(this);

        setLookAndFeel(deviceSelectionLookAndFeel);
    }

    ~DeviceSelectionMenu() override
    {
        audioDeviceManager.removeChangeListener(this);
        delete innerComponent;

        setLookAndFeel(nullptr);
        delete deviceSelectionLookAndFeel;
    }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void closeButtonPressed() override
    {
        removeFromDesktop(); //maybe?
    }

    //ApplicationCommandTarget methods
    ApplicationCommandTarget *getNextCommandTarget() override
    {
        return nullptr;
    }

    void getAllCommands(juce::Array<juce::CommandID> &c) override
    {
        juce::Array<juce::CommandID> commands{
            SP_CommandID::show_audio_device_menu
        };
        c.addArray(commands);
    }

    void getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result) override
    {
        switch (commandID)
        {
            case SP_CommandID::show_audio_device_menu:
                result.setInfo("Show audio device menu", "Show/hides menu", "menu", 0);
                result.setTicked(false);
            //result.setActive(true);
                result.addDefaultKeypress('l', juce::ModifierKeys::commandModifier);
            //commandManager.commandStatusChanged();
                break;
            default:
                break;
        }
    }

    bool perform(const InvocationInfo &info) override
    {
        switch (info.commandID)
        {
            case SP_CommandID::show_audio_device_menu:
                addToDesktop();
                setVisible(true);
                break;
            default:
                return false;
        }

        return true;
    }

    //

    class InnerComponent final : public juce::Component
    {
    public:
        explicit InnerComponent(juce::AudioDeviceManager& manager): audioDeviceManager(manager)
        {

            audioSetupComp.reset(new juce::AudioDeviceSelectorComponent(audioDeviceManager,
                                                                        0, 256, 0, 256, true, true, true, false));



            addAndMakeVisible(audioSetupComp.get());
            addAndMakeVisible(diagnosticsBox);

            diagnosticsBox.setMultiLine(true);
            diagnosticsBox.setReturnKeyStartsNewLine(true);
            diagnosticsBox.setReadOnly(true);
            diagnosticsBox.setScrollbarsShown(true);
            diagnosticsBox.setCaretVisible(false);
            diagnosticsBox.setPopupMenuEnabled(true);

            setSize(500, 600);

            dumpDeviceInfo();

            logMessage("Audio device diagnostics:\n");
        }

        ~InnerComponent() override = default;

        void resized() override
        {
            auto r = getLocalBounds().reduced(4);
            audioSetupComp->setBounds(r.removeFromTop(proportionOfHeight(0.65f)));
            diagnosticsBox.setBounds(r);
        }

        void lookAndFeelChanged() override
        {
            diagnosticsBox.applyFontToAllText(diagnosticsBox.getFont());
        }

        void logMessage(const juce::String &m)
        {
            diagnosticsBox.moveCaretToEnd();
            diagnosticsBox.insertTextAtCaret(m + juce::newLine);
        }

        void dumpDeviceInfo()
        {
            logMessage("--------------------------------------");
            logMessage("Current audio device type: " + (audioDeviceManager.getCurrentDeviceTypeObject() != nullptr
                                                            ? audioDeviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                            : "<none>"));

            if (juce::AudioIODevice *device = audioDeviceManager.getCurrentAudioDevice())
            {
                logMessage("Current audio device: " + device->getName().quoted());
                logMessage("Sample rate: " + juce::String(device->getCurrentSampleRate()) + " Hz");
                logMessage("Block size: " + juce::String(device->getCurrentBufferSizeSamples()) + " samples");
                logMessage("Output Latency: " + juce::String(device->getOutputLatencyInSamples()) + " samples");
                logMessage("Input Latency: " + juce::String(device->getInputLatencyInSamples()) + " samples");
                logMessage("Bit depth: " + juce::String(device->getCurrentBitDepth()));
                logMessage("Input channel names: " + device->getInputChannelNames().joinIntoString(", "));
                logMessage("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()));
                logMessage("Output channel names: " + device->getOutputChannelNames().joinIntoString(", "));
                logMessage("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()));
            } else
            {
                logMessage("No audio device open");
            }
        }

    private:
        std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSetupComp;
        juce::TextEditor diagnosticsBox;

        juce::AudioDeviceManager& audioDeviceManager;
    };

private:
    SPCommandManager& commandManager;

    juce::AudioDeviceManager& audioDeviceManager;

    InnerComponent* innerComponent;

    juce::LookAndFeel_V4* deviceSelectionLookAndFeel{new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getGreyColourScheme())};


    void changeListenerCallback(juce::ChangeBroadcaster *) override
    {
        innerComponent->dumpDeviceInfo();
    }


    static juce::String getListOfActiveBits(const juce::BigInteger &b)
    {
        juce::StringArray bits;

        for (int i = 0; i <= b.getHighestBit(); ++i)
            if (b[i])
                bits.add(juce::String(i));

        return bits.joinIntoString(", ");
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSelectionMenu)
};
