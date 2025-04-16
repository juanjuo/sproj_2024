//
// Created by Juan Diego on 10/6/24.
//

#include <MainWindow.h>

MainWindow::MainWindow (const juce::String& name, MainComponent* mainComponent) : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour
                           (ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (mainComponent, true);

    setResizable (true, true);
    centreWithSize (getWidth(), getHeight());
    setFullScreen(true);

    MainWindow::setVisible (true);

    //MELATONIN WINDOW
    // open the inspector window
    //inspector.setVisible(this);

    // enable the inspector
    //inspector.toggle(this);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

