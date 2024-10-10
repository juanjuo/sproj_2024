//
// Created by Juan Diego on 10/9/24.
//

#include <juce_gui_extra/juce_gui_extra.h>

/*
 * GUI class that handles the initialization and interactions with the Clock class
 */

class ClockGUI : public juce::Component
{
  public:

    ClockGUI()
    {
      tempoNumerator.setFont(font); //font
      tempoDenominator.setFont(font);
      BPM.setFont(font);

      //maybe using tempoNumerator.setJustification?
      tempoNumerator.setIndents(10, 0); // margin
      tempoDenominator.setIndents(10, 0);
      BPM.setIndents(10, 0);

      tempoNumerator.setTextToShowWhenEmpty(std::to_string(*tempo_num_val), juce::Colours::white);
      tempoDenominator.setTextToShowWhenEmpty(std::to_string(*tempo_num_val), juce::Colours::white);
      BPM.setTextToShowWhenEmpty(std::to_string(*bpm), juce::Colours::white);

      tempoNumerator.setReturnKeyStartsNewLine(false); //no new line
      tempoDenominator.setReturnKeyStartsNewLine(false);
      BPM.setReturnKeyStartsNewLine(false);

      //tempoNumerator.setCaretVisible(false);
      //tempoDenominator.setCaretVisible(false);

      tempoNumerator.setInputRestrictions(1); //only 1 input
      tempoDenominator.setInputRestrictions(1);
      BPM.setInputRestrictions(3);

      tempoNumerator.onReturnKey = [this]() //using lambda when ENTER key pressed
      {
        *tempo_num_val = tempoNumerator.getText().getIntValue();
        std::cout << std::to_string(*tempo_num_val) << '\n';
      };

      tempoDenominator.onReturnKey = [this]()
      {
        *tempo_den_val = tempoDenominator.getText().getIntValue();
        std::cout << std::to_string(*tempo_den_val) << '\n';
      };

      BPM.onReturnKey = [this]()
      {
        *bpm = BPM.getText().getIntValue();
        std::cout << std::to_string(*bpm) << '\n';
      };

      addAndMakeVisible(tempoNumerator); //initialize
      addAndMakeVisible(tempoDenominator);
      addAndMakeVisible(BPM);
    }

    void resized() override //position on screen
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
    int* tempo_num_val = new int{4};
    int* tempo_den_val = new int{4};
    int* bpm = new int{120};

    juce::FontOptions font = juce::FontOptions(50.0, juce::Font::bold); //font
    juce::Font clockFont = juce::Font(font);
    juce::TextEditor tempoNumerator; //text fields
    juce::TextEditor tempoDenominator;
    juce::TextEditor BPM;
};


