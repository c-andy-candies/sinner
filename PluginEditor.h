#pragma once

#include "PluginProcessor.h"
#include "MatrixPage.cpp"
#include "OscillatorPage.cpp"

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

    //TODO: Should probably put them in an array with length NUMBER_OF_OSCILLATORS
    juce::TextButton sideItemOscA;
    juce::TextButton sideItemOscB;

    juce::TextButton footer;
 
    MatrixPage matrixPage;
    OscillatorPage oscillatorPage;

    juce::Component* currentPage;

    void switchToMatrixPage()
    {
        currentPage->setVisible(false);
        currentPage = &matrixPage;
        currentPage->setVisible(true);
        resized();
    }

    void switchToOscillatorPage()
    {
        currentPage->setVisible(false);
        currentPage = &oscillatorPage;
        currentPage->setVisible(true);
        resized();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
