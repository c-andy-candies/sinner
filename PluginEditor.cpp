#include "PluginProcessor.h"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), matrixPage(processorRef), oscillatorPage(processorRef)
{

    juce::ignoreUnused (processorRef);

    header.setColour (juce::TextButton::buttonColourId, juce::Colours::cornflowerblue);
    header.setButtonText ("sin(ner)");
    addAndMakeVisible (header);

    footer.setColour (juce::TextButton::buttonColourId, juce::Colours::cornflowerblue);
    footer.setButtonText ("Created by c-andy");
    addAndMakeVisible (footer);

    sidebar.setColour (juce::TextButton::buttonColourId, juce::Colours::grey);
    addAndMakeVisible (sidebar);

    sideItemMatrix.setColour (juce::TextButton::buttonColourId, juce::Colours::grey);
    sideItemMatrix.setButtonText("Modulation Matrix");
    sideItemMatrix.onClick = [this] { switchToMatrixPage(); };

    sideItemOscA.setColour (juce::TextButton::buttonColourId, juce::Colours::grey);
    sideItemOscA.setButtonText("Oscillator A");
    sideItemOscA.onClick = [this] { switchToOscillatorPage(); };

    sideItemOscB.setColour (juce::TextButton::buttonColourId, juce::Colours::grey);
    sideItemOscB.setButtonText("Oscillator B");

    addAndMakeVisible (sideItemMatrix);
    addAndMakeVisible (sideItemOscA);
    addAndMakeVisible (sideItemOscB);

    addAndMakeVisible(&matrixPage);
    addAndMakeVisible(&oscillatorPage);

    currentPage = &matrixPage;

    //TODO: Hardcoded value
    setSize (400, 400);

}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void AudioPluginAudioProcessorEditor::resized()
{

    auto area = getLocalBounds();
 
    //TODO: Hardcoded value
    auto headerFooterHeight = 36;
    header.setBounds (area.removeFromTop    (headerFooterHeight));
    footer.setBounds (area.removeFromBottom (headerFooterHeight));
 
    auto sideBarArea = area.removeFromRight (juce::jmax (80, area.getWidth() / 4));
    sidebar.setBounds (sideBarArea);
 
    //TODO: Hardcoded value
    auto sideItemHeight = 40;
    //TODO: Hardcoded value
    auto sideItemMargin = 5;
    sideItemMatrix.setBounds (sideBarArea.removeFromTop (sideItemHeight).reduced (sideItemMargin));
    sideItemOscA.setBounds (sideBarArea.removeFromTop (sideItemHeight).reduced (sideItemMargin));
    sideItemOscB.setBounds (sideBarArea.removeFromTop (sideItemHeight).reduced (sideItemMargin));

    currentPage->setBounds(area);
 
}