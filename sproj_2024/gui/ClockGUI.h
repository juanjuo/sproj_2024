//
// Created by Juan Diego on 10/9/24.
//

#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <Clock.h>

class ClockGUI : public juce::Component
{
public:
    explicit ClockGUI(juce::ValueTree& valueTree);

    void resized() override;


    int* getTempoNumVal();
    int* getTempoDenVal();
    int* getBpm();

    void updateController();

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

    //Clock clock;
};