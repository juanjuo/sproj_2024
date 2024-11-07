//
// Created by Juan Diego on 10/9/24.
//

#include <ClockGUI.h>
#include <Identifiers.h>


/*
 * GUI class that handles the initialization and interactions with the Clock class
 */

// Constructor
ClockGUI::ClockGUI(juce::ValueTree& valueTree) : clockGuiValueTree(valueTree)
{

  //clock.startTimer(clock.toMilliseconds(*bpm));

  tempoNumerator.setFont(font);
  tempoDenominator.setFont(font);
  BPM.setFont(font);

  tempoNumerator.setIndents(10, 0);
  tempoDenominator.setIndents(10, 0);
  BPM.setIndents(10, 0);

  tempoNumerator.setTextToShowWhenEmpty(std::to_string(*tempo_num_val), juce::Colours::white);
  tempoDenominator.setTextToShowWhenEmpty(std::to_string(*tempo_den_val), juce::Colours::white);
  BPM.setTextToShowWhenEmpty(std::to_string(*bpm), juce::Colours::white);

  tempoNumerator.setReturnKeyStartsNewLine(false);
  tempoDenominator.setReturnKeyStartsNewLine(false);
  BPM.setReturnKeyStartsNewLine(false);

  tempoNumerator.setInputRestrictions(1);
  tempoDenominator.setInputRestrictions(1);
  BPM.setInputRestrictions(3);

  tempoNumerator.onReturnKey = [this]()
  {
    *tempo_num_val = tempoNumerator.getText().getIntValue();
    if (clockGuiValueTree.isValid()) {
      clockGuiValueTree.setProperty(SP_ID::numerator, tempoNumerator.getTextValue(), nullptr );
      std::cout << std::to_string(*tempo_num_val) + " from clockGUI" << '\n';
    }
  };

  tempoDenominator.onReturnKey = [this]()
  {
    *tempo_den_val = tempoDenominator.getText().getIntValue();
    clockGuiValueTree.setProperty(SP_ID::denominator, tempoDenominator.getTextValue(), nullptr );
    std::cout << std::to_string(*tempo_den_val) + " from clockGUI" << '\n';
  };

  BPM.onReturnKey = [this]()
  {
    *bpm = BPM.getText().getIntValue();
    clockGuiValueTree.setProperty(SP_ID::bpm, BPM.getTextValue(), nullptr );
    std::cout << std::to_string(*bpm) + " from clockGUI"<< '\n';
  };

  addAndMakeVisible(tempoNumerator);
  addAndMakeVisible(tempoDenominator);
  addAndMakeVisible(BPM);
}

// Resized method for positioning on the screen
void ClockGUI::resized()
{
  tempoNumerator.setBounds(0, 0, 50, 50);
  tempoDenominator.setBounds(50, 0, 50, 50);
  BPM.setBounds(100, 0, 100, 50);
}

// Getter for tempo numerator value
int* ClockGUI::getTempoNumVal()
{
  return tempo_num_val;
}

// Getter for tempo denominator value
int* ClockGUI::getTempoDenVal()
{
  return tempo_den_val;
}

// Getter for BPM
int* ClockGUI::getBpm()
{
  return bpm;
}


