//
// Created by Juan Diego on 12/30/24.
//

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <Identifiers.h>

//Metronome class
class ClockGUI final : public juce::Component
{
public:
  explicit ClockGUI(juce::ValueTree& valueTree) : clockGuiValueTree(valueTree)
  {
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

  void resized() override
  {
    tempoNumerator.setBounds(0, 0, 50, 50);
    tempoDenominator.setBounds(50, 0, 50, 50);
    BPM.setBounds(100, 0, 100, 50);
  }


  int* getTempoNumVal()
  {
    return tempo_num_val;
  }

  int* getTempoDenVal()
  {
    return tempo_den_val;
  }

  int* getBpm()
  {
    return bpm;
  }

private:
  juce::ValueTree clockGuiValueTree;

  int* tempo_num_val = new int{4};
  int* tempo_den_val = new int{4};
  int* bpm = new int{120};

  juce::FontOptions font = juce::FontOptions(50.0, juce::Font::bold);
  juce::Font clockFont = juce::Font(font);
  juce::TextEditor tempoNumerator;
  juce::TextEditor tempoDenominator;
  juce::TextEditor BPM;
};

//Component that holds all the smaller parts of the Control Deck
class ControlDeckGUI final : public juce::Component
{
public:

  explicit ControlDeckGUI(juce::ValueTree& valueTree) : clockGui(valueTree)
  {
  }

  void paint(juce::Graphics &g) override
  {
    addAndMakeVisible(clockGui);
  }

  void resized() override
  {
    clockGui.setBounds(0, 0, 200, 400);
  }

private:

  ClockGUI clockGui; //Metronome

};




