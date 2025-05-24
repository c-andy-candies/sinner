#include <JuceHeader.h>
#include <iostream>
#include <set>
#include <iterator>
#include "PluginProcessor.h"
#include "EnvelopeRange.cpp"
#include "constants.h"
#include "OscillatorView.cpp"
#include "juce_graphics/juce_graphics.h"

class EnvelopeRangesEditor : public juce::Component
{
public:
    EnvelopeRangesEditor(AudioPluginAudioProcessor& p)
        : processorRef(p) 
    {}

    void paint(juce::Graphics& g) override
    {
        drawEnvelope(g);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {

        auto width = getWidth();

        auto clicked_time = static_cast<float>(event.x) / width;
        
        // Check if the user clicked near an existing range
        for (auto it = currentOsc->getEnvelopeRangesBegin(); it != currentOsc->getEnvelopeRangesEnd(); ++it)
        {

            //TODO: set the threshold to select a range
            if (std::abs(clicked_time - it->getTime()) < 0.01f)
            {
                //TODO: UX-wise, is this the best way to delete a range?
                //Delete the range if the CTRL-key is pressed
                if (event.mods.isCtrlDown())
                {
                    processorRef.removeEnvelopeRange(currentOsc->getOscNumber(), it->getRangeNumber());
                    currentOsc->removeEnvelopeRange(it);
                    repaint();
                } 
                else 
                {
                    // Select this range for dragging
                    selectedRangeIndex = it; // Store the iterator to the selected range
                }

                return;
            }
        }

        if (currentOsc->getEnvelopeRangesSize() < NUMBER_OF_ENVELOPE_RANGES) {
            
            //TODO: only create a new range if the closest point to it is not already "occupied"

            auto range_time = currentOsc->findClosestPointValue(clicked_time);

            //TODO: maybe instead we could always force the program to always have a point
            //Check whether at least one point exists
            if (range_time != 0.0f)
            {

                // If no range was selected or deleted, add a new range
                auto [range_number, time, reps] = processorRef.addEnvelopeRange(currentOsc->getOscNumber(), range_time);
            
                EnvelopeRange new_range = EnvelopeRange(range_number, time, reps); 
                currentOsc->addEnvelopeRange(new_range);

                //TODO: Probably could select here the newly created range

                repaint();
            }

        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {

        if (selectedRangeIndex != currentOsc->getEnvelopeRangesEnd())
        {
            auto width = getWidth();

            auto clicked_time = currentOsc->findClosestPointValue(static_cast<float>(event.x) / width);
            
            float min_time;
       
            if (selectedRangeIndex != currentOsc->getEnvelopeRangesBegin())
            {
                min_time = currentOsc->getRangeMinTime(std::prev(selectedRangeIndex)->getTime());
            } 
            else 
            {
                min_time = currentOsc->getFirstPointTime();
            }

            auto next_range = std::next(selectedRangeIndex);
            float max_time;

            if (next_range != currentOsc->getEnvelopeRangesEnd())
            {
                max_time = currentOsc->getRangeMaxTime(next_range->getTime());
            }
            else
            {
                max_time = currentOsc->getLastPointTime();
            }

            // Constrain the new position within the boundaries
            auto new_time = jlimit(min_time, max_time, clicked_time);

            processorRef.moveEnvelopeRange(currentOsc->getOscNumber(), selectedRangeIndex->getRangeNumber(),  new_time);

            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        selectedRangeIndex = currentOsc->getEnvelopeRangesEnd(); // Deselect the range
    }

    void setOscView(OscillatorView* osc_view)
    {
        currentOsc = osc_view;
        selectedRangeIndex = currentOsc->getEnvelopeRangesEnd();
        repaint();
    }

private:

    AudioPluginAudioProcessor& processorRef;

    OscillatorView* currentOsc;

    std::set<EnvelopeRange>::iterator selectedRangeIndex; // Iterator to the selected point

    void drawEnvelope(juce::Graphics& g)
    {

        g.fillAll(juce::Colours::beige);

        auto width = getWidth();

        if (!currentOsc->isEnvelopeRangesEmpty())
        {

            auto it = currentOsc->getEnvelopeRangesBegin();

            while (it != currentOsc->getEnvelopeRangesEnd())
            {
                auto it_width =  it->getTime() * width;

                g.drawLine(it_width, 0.0, it_width, getHeight());

                ++it;
            }

        }
    }
};