#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(juce::ValueTree tree, SPCommandManager& manager)
    : controlDeckGui(tree), mainDeckGui(tree), freeDeckGui(tree), mixDeckGui(tree), menu(manager)

{
    if (tree.isValid()) std::cout << "is valid" << std::endl;
    setSize(600, 400);

    addAndMakeVisible(controlDeckGui);
    addAndMakeVisible(freeDeckGui);
    addAndMakeVisible(mixDeckGui);
    addAndMakeVisible(mainDeckGui);
    addAndMakeVisible(menu);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    controlDeckGui.setBounds(getX(), getY(), getWidth(), controlDeckGui.getHeight());
    freeDeckGui.setBounds(getX(), getHeight() - freeDeckGui.getHeight(), getWidth(), freeDeckGui.getHeight());
    mixDeckGui.setBounds(getX(), getY() + controlDeckGui.getHeight(), mixDeckGui.getWidth(), getHeight()
                         - controlDeckGui.getHeight() - freeDeckGui.getHeight());
    mainDeckGui.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
                            - mixDeckGui.getWidth(), getHeight() - freeDeckGui.getHeight() - controlDeckGui.getHeight());
}

void MainComponent::childBoundsChanged(Component *child)
{
    controlDeckGui.setBounds(getX(), getY(), getWidth(), controlDeckGui.getHeight());
    freeDeckGui.setBounds(getX(), getHeight() - freeDeckGui.getHeight(), getWidth(), freeDeckGui.getHeight());
    mixDeckGui.setBounds(getX(), getY() + controlDeckGui.getHeight(), mixDeckGui.getWidth(), getHeight()
                         - controlDeckGui.getHeight() - freeDeckGui.getHeight());
    mainDeckGui.setBounds(getX() + mixDeckGui.getWidth(), getY() + controlDeckGui.getHeight(), getWidth()
                            - mixDeckGui.getWidth(), getHeight() - freeDeckGui.getHeight() - controlDeckGui.getHeight());
}

