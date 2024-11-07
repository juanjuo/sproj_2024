#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::ValueTree tree) : clockGui(tree)
{
    if(tree.isValid()) std::cout<<"is valid"<<std::endl;
    setSize (600, 400);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setFont (juce::FontOptions (16.0f));
    // g.setColour (juce::Colours::white);
    // g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);

    addAndMakeVisible(clockGui);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    clockGui.setBounds(getWidth()/2 - 100, getHeight()/2 - 100, 200, 50);
}
