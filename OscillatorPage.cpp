#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "constants.h"
#include "EnvelopeEditor.cpp"

class OscillatorPage : public juce::Component,
                   public juce::Slider::Listener
{
public:
    OscillatorPage(AudioPluginAudioProcessor& p) : processorRef (p), envelope(processorRef)
    {

        juce::ignoreUnused (processorRef);

        for(auto i = 0; i < NUMBER_OF_HARMONICS; ++i)
        {

            harmonicVolumeSlider[i].setSliderStyle(juce::Slider::LinearVertical);
            harmonicVolumeSlider[i].setRange (0.0, 1.0);
            harmonicVolumeSlider[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // Hide the text box
            harmonicVolumeSlider[i].addListener (this);
            addAndMakeVisible(harmonicVolumeSlider[i]);

        }

        addAndMakeVisible(envelope);

    }

    void resized() override
    {
        
        auto area = getLocalBounds();

        //TODO: Hardcoded value
        auto border = 10;

        envelope.setBounds (area.removeFromTop    (area.getHeight() / 2 - border));

        auto element_height = (area.getHeight() - border);
        auto element_width = (area.getWidth() - border) / (NUMBER_OF_HARMONICS);
        
        for(auto i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
        {
            harmonicVolumeSlider[i].setBounds(i * element_width, element_height, element_width, element_height);
        }
    }

void sliderValueChanged (juce::Slider* slider) override
{

    auto new_value = slider->getValue();

    // Loop through the matrix to find which slider changed
    // TODO: could store all sliders in an hashtable with index the &slider address thus reducing it to O(1)
    for (auto i = 0; i < NUMBER_OF_HARMONICS; ++i)
    {
        if (slider == &harmonicVolumeSlider[i])
        {
            //Call the method to actually update the value
            processorRef.setHarmonicVolume(0, i, new_value);
            return;
        }
        
    }

    //If no slider has been found then return 
    // TODO: Should have some error reporting instead
    return;
}
private:

    AudioPluginAudioProcessor& processorRef;

    juce::Slider harmonicVolumeSlider[NUMBER_OF_HARMONICS];

    EnvelopeEditor envelope;
};
