#include <JuceHeader.h>
#include <set>
#include <iterator>
#include "PluginProcessor.h"
#include "EnvelopePoint.cpp"
#include "constants.h"
#include "OscillatorView.cpp"

class EnvelopeEditor : public juce::Component
{
public:
    EnvelopeEditor(AudioPluginAudioProcessor& p)
        : processorRef(p) 
    {}

    void paint(juce::Graphics& g) override
    {
        drawEnvelope(g);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {

        auto width = getWidth();
        auto height = getHeight();

        auto clicked_time = static_cast<float>(event.x) / width;
        auto clicked_amp = static_cast<float>(event.y) / height;
        
        // Check if the user clicked near an existing point
        for (auto it = currentOsc->getEnvelopePointsBegin(); it != currentOsc->getEnvelopePointsEnd(); ++it)
        {

            //TODO: set the threshold to select a point, it should be the actual size of the point
            if (std::abs(clicked_time - it->getTime()) < 0.01f && std::abs(clicked_amp - it->getAmplitude()) < 0.01f)
            {

                //TODO: UX-wise, is this the best way to delete a point?
                //Delete the point if the CTRL-key is pressed
                if (event.mods.isCtrlDown())
                {
                    processorRef.removeEnvelopePoint(currentOsc->getOscNumber(), it->getPointNumber());
                    currentOsc->removeEnvelopePoint(it);
                    repaint();
                } 
                else 
                {
                    // Select this point for dragging
                    selectedPoint = *it; // Store the selected point
                    selectedPointIndex = it; // Store the iterator to the selected point
                }

                return;
            }
        }

        if (currentOsc->getEnvelopePointsSize() < NUMBER_OF_ENVELOPE_POINTS) {
            
            // If no point was selected or deleted, add a new point
            auto [pointNumber, time, amplitude, interpolation] = processorRef.addEnvelopePoint(currentOsc->getOscNumber(), clicked_time, clicked_amp);

            EnvelopePoint new_point = EnvelopePoint(pointNumber, time, amplitude, interpolation); 
            currentOsc->addEnvelopePoint(new_point);

            //TODO: Probably could select here the newly created point

            repaint();

        }
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {

        if (selectedPointIndex != currentOsc->getEnvelopePointsEnd())
        {
            auto width = getWidth();
            auto height = getHeight();

            auto clicked_time = static_cast<float>(event.x) / width;
            auto clicked_amp = static_cast<float>(event.y) / height;
            
            float min_time;

            if (selectedPointIndex != currentOsc->getEnvelopePointsBegin())
            {
                min_time = std::prev(selectedPointIndex)->getTime();
            } 
            else 
            {
                min_time = 0.0f;
            }

            auto next_point = std::next(selectedPointIndex);
            float max_time;

            if (next_point != currentOsc->getEnvelopePointsEnd())
            {
                max_time = next_point->getTime();
            }
            else
            {
                max_time = 1.0f;
            }

            // Constrain the new position within the boundaries
            auto new_time = jlimit(min_time, max_time, clicked_time);

            processorRef.moveEnvelopePoint(currentOsc->getOscNumber(), selectedPoint.getPointNumber(),  new_time, clicked_amp);

            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        selectedPointIndex = currentOsc->getEnvelopePointsEnd(); // Deselect the point
    }

    void setOscView(OscillatorView* osc_view)
    {
        currentOsc = osc_view;
        selectedPointIndex = currentOsc->getEnvelopePointsEnd();
        repaint();
    }

private:

    AudioPluginAudioProcessor& processorRef;

    OscillatorView* currentOsc;

    std::set<EnvelopePoint>::iterator selectedPointIndex; // Iterator to the selected point
    EnvelopePoint selectedPoint; // Variable to hold the currently selected point's coordinates

    void drawEnvelope(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::white);

        auto width = getWidth();
        auto height = getHeight();

        if (!currentOsc->isEnvelopePointsEmpty())
        {
            auto firstPoint = *currentOsc->getEnvelopePointsBegin();
            auto firstPoint_width = firstPoint.getTime() * width;
            auto firstPoint_height = firstPoint.getAmplitude() * height;
            g.drawLine(0.0f, (float)height, firstPoint_width, firstPoint_height);

            auto midX = (0.0f + firstPoint_width) / 2;
            auto midY = (float)height + (firstPoint_height - height) / 2; // Midpoint Y calculation
            g.drawEllipse(midX - 5, midY - 5, 10, 10, 1.0f); // Draw the curvature control point

            auto it = currentOsc->getEnvelopePointsBegin();
            auto prev = it;
            ++it;

            while (it != currentOsc->getEnvelopePointsEnd())
            {
                auto prev_width = prev->getTime() * width;
                auto prev_height = prev->getAmplitude() * height;
                auto it_width =  it->getTime() * width;
                auto it_height = it->getAmplitude() * height;

                g.drawLine(prev_width, prev_height, it_width, it_height);

                // TODO: To be modified when other curvature methods are defined
                auto midX_ = (prev_width + it_width) / 2;
                auto midY_ = (prev_height + it_height) / 2;

                g.drawEllipse(midX_ - 5, midY_ - 5, 10, 10, 1.0f);

                prev = it;
                ++it;
            }

            auto prev_width = prev->getTime() * width;
            auto prev_height = prev->getAmplitude() * height;

            g.drawLine(prev_width, prev_height, (float)width, (float)height);

            auto mid_X = (prev_width + (float)width) / 2;
            auto mid_Y = (prev_height + (float)height) / 2;
            g.drawEllipse(mid_X - 5, mid_Y - 5, 10, 10, 1.0f);

            for (const auto& point : currentOsc->getEnvelopePoints())
            {
                auto point_width = point.getTime() * width;
                auto point_height = point.getAmplitude() * height;
                g.fillEllipse(point_width - 5, point_height - 5, 10, 10);
            }
        }
    }
};