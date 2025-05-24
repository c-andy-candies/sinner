#pragma once

#include <vector>
#include "PluginProcessor.h"
#include "MatrixPage.cpp"
#include "OscillatorPage.cpp"
#include "OscillatorView.cpp"
#include "constants.h"

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
private:

    AudioPluginAudioProcessor& processorRef;

    juce::TextButton header;
    juce::TextButton sidebar;
 
    juce::TextButton sideItemMatrix;

    std::array<juce::TextButton, NUMBER_OF_OSCILLATORS> sideItemOscs;

    juce::TextButton footer;
 
    MatrixPage matrixPage;
    OscillatorPage oscillatorPage;

    juce::Component* currentPage;

    std::vector<OscillatorView> oscViews;

    void switchToMatrixPage()
    {
        currentPage->setVisible(false);
        currentPage = &matrixPage;
        currentPage->setVisible(true);
        resized();
    }

    void switchToOscillatorPage(int osc)
    {
        currentPage->setVisible(false);
        currentPage = &oscillatorPage;
        oscillatorPage.setOscView(&oscViews[osc]);
        currentPage->setVisible(true);
        resized();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
