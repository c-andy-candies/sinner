#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "constants.h"

class MatrixPage : public juce::Component,
                   public juce::Slider::Listener
{
public:
    MatrixPage(AudioPluginAudioProcessor& p) : processorRef (p)
    {

        juce::ignoreUnused (processorRef);

        for(auto i = 0; i < NUMBER_OF_OSCILLATORS + 1; ++i)
        {
            for(auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
            {

                FM_index_slider[i][j].setSliderStyle(juce::Slider::LinearVertical);
                FM_index_slider[i][j].setRange (0.0, 1.0);

                FM_index_slider[i][j].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // Hide the text box
                FM_index_slider[i][j].addListener (this);
                addAndMakeVisible(FM_index_slider[i][j]);

            }
        }
    }

    void resized() override
    {
        
        auto area = getLocalBounds();
    
        auto matrix_border = 10;
        auto matrix_element_height = (area.getHeight() - matrix_border) / (NUMBER_OF_OSCILLATORS + 1);
        auto matrix_element_width = (area.getWidth() - matrix_border) / (NUMBER_OF_OSCILLATORS);
        
        for(auto i = 0; i < NUMBER_OF_OSCILLATORS + 1; ++i)
        {
            for(auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
            {
                FM_index_slider[i][j].setBounds(j * matrix_element_width, (i + 1) * matrix_element_height, matrix_element_width, matrix_element_height);

                FM_index_label[i][j].setBounds(j * matrix_element_width, (i + 1) * matrix_element_height, matrix_element_width, matrix_element_height);

            }
        }
    }

void sliderValueChanged (juce::Slider* slider) override
{

    auto new_value = slider->getValue();

    // Loop through the matrix to find which slider changed
    // TODO: could store all sliders in an hashtable with index the &slider address thus reducing it to O(1)
    for (auto i = 0; i < NUMBER_OF_OSCILLATORS + 1; ++i)
    {
        for (auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
        {
            if (slider == &FM_index_slider[i][j])
            {
                FM_index_label[i][j].setText(juce::String(new_value), juce::dontSendNotification);

                //Call the method to actually update the value
                processorRef.setFMIndex(i, j, new_value);
                return;
            }
        }
    }

    //If no slider has been found then return
    // TODO: Should have some error reporting instead
    return;
}
private:

    AudioPluginAudioProcessor& processorRef;

    juce::Slider FM_index_slider[NUMBER_OF_OSCILLATORS + 1][NUMBER_OF_OSCILLATORS];
    juce::Label  FM_index_label[NUMBER_OF_OSCILLATORS + 1][NUMBER_OF_OSCILLATORS];
};
