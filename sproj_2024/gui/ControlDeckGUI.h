//
// Created by Juan Diego on 12/30/24.
//

#pragma once
#include <DeckGUI.h>
#include <helpers.h>
#include "focusrite/e2e/ComponentSearch.h"

//Metronome class
class ClockGUI final : public juce::Component,
                       public juce::SliderListener<juce::Slider>
{
public:
  explicit ClockGUI(const juce::ValueTree& tree, const int height)
    : TEXTBOX_HEIGHT(height / 2), valueTree(tree.getChildWithName(SP_ID::METRONOME_BRANCH))
  {
    configureTextEditor(textEditorGrouping, std::to_string(grouping));
    configureTextEditor(textEditorBPM, std::to_string(bpm));

    //Different restrictions
    textEditorGrouping.setInputRestrictions(1);
    textEditorBPM.setInputRestrictions(3);

    configureTextEditorCallback(textEditorGrouping, grouping, SP_ID::metronome_grouping);
    configureTextEditorCallback(textEditorBPM, bpm, SP_ID::metronome_bpm);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    volumeSlider.setBounds(getLocalBounds().reduced(5));
    volumeSlider.setRange(0.0, 2.0, 0.1);
    volumeSlider.setValue(initialGain);
    volumeSlider.addListener(this);

    focusrite::e2e::ComponentSearch::setTestId (volumeSlider, "test_metronome_volume_slider"); //for testing

    initializeValueTree();
  }

  void resized() override
  {
    textEditorBPM.setBounds(CLOCK_MARGIN, 0, BPM_WIDTH, TEXTBOX_HEIGHT);
    textEditorGrouping.setBounds(CLOCK_MARGIN + textEditorBPM.getWidth(), 0, TEXTBOX_HEIGHT, TEXTBOX_HEIGHT);
    volumeSlider.setBounds(0, TEXTBOX_HEIGHT, getWidth(), TEXTBOX_HEIGHT);
  }

  void configureTextEditor(juce::TextEditor& editor, const juce::String& defaultText) {
    editor.setFont(FONT);
    editor.setJustification(JUSTIFICATION);
    editor.setTextToShowWhenEmpty(defaultText, juce::Colours::white);
    editor.setReturnKeyStartsNewLine(false);
    editor.setCaretVisible(false);
    editor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    editor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(editor);
  }

  void configureTextEditorCallback(juce::TextEditor& editor, int& value, const juce::Identifier& propertyId)
  {
    editor.onReturnKey = [this, &editor, &value, propertyId]() {
      value = editor.getText().getIntValue(); //Add bounds
      if (valueTree.isValid()) {
        valueTree.setProperty(propertyId, editor.getTextValue(), nullptr);
        std::cout << std::to_string(value) + " from clockGUI\n";
      }
    };
  }

  void initializeValueTree()
  {
    valueTree.setProperty(SP_ID::metronome_bpm, bpm, nullptr);
    valueTree.setProperty(SP_ID::metronome_grouping, grouping, nullptr);
    valueTree.setProperty(SP_ID::metronome_gain, initialGain, nullptr);
  }


  int getGroupingNum()
  {
    return grouping;
  }

  int getBpm() const
  {
    return bpm;
  }

  //Slider Listener

  void sliderValueChanged(juce::Slider* slider) override
  {
    valueTree.setProperty(SP_ID::metronome_gain, slider->getValue(), nullptr);
  }

private:
  //CLOCK POSITIONING
  const int TEXTBOX_HEIGHT;
  const int BPM_WIDTH = 100;
  const int CLOCK_MARGIN = 20;
  const float FONT_SIZE = 30.0f;
  const juce::Justification JUSTIFICATION = {juce::Justification::centred};

  //FONT
  juce::FontOptions FONT = juce::FontOptions(FONT_SIZE, juce::Font::bold);
  juce::Font clockFont = juce::Font(FONT);

  juce::ValueTree valueTree;

  //ValueTree VALS
  int grouping = 4;
  int bpm = 120;

  juce::TextEditor textEditorGrouping;
  juce::TextEditor textEditorBPM;

  float initialGain = 0.5f;

  //volume slider
  juce::Slider volumeSlider;
};

//Component that holds all the smaller parts of the Control Deck
class ControlDeckGUI final : public juce::Component,
                             public DeckGUI

{
public:

  explicit ControlDeckGUI(const juce::ValueTree& valueTree)
    : DeckGUI(200, 80, juce::Colour::fromRGB(15, 15, 15)), clockGui(valueTree, 80)
  {
    setSize(WINDOW_HEIGHT, WINDOW_HEIGHT);
  }

  void paint(juce::Graphics &g) override
  {
    g.fillAll(BACKGROUND_COLOUR);
    g.drawRect(this->getLocalBounds(), BORDER_WIDTH);
    addAndMakeVisible(clockGui);
  }

  void resized() override
  {
    clockGui.setBounds(getLocalBounds());
  }

private:

  //Metronome
  ClockGUI clockGui;
};




