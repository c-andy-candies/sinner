#pragma once

#include "SinnerSound.cpp"
#include "Matrix.hpp"

class SinnerVoice : public juce::SynthesiserVoice
{
public:
    SinnerVoice (Matrix& mod_matrix, const int voice_id_)
        : matrix (mod_matrix), voice_id(voice_id_) 
    {

    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SinnerSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        
        //TODO: It should call setFrequency of every harmonic to set note freq
        //But when there is a modwheel change it should change again
        //Maybe we should parametrize getSample on note

        //TODO: Also check about the relation between reading LUT and sampleRate of plugin
        //In Sinner.hpp we also divide by SIN_FREQUENCY -> Necessary? (If it is we can play with the tuning instead -> since SIN_FREQUENCY is 10 we can request a tuning of 44 instead of 440 and voilà)
        //In constants.h we solved the division by SIN_FREQUENCY
        matrix.startingVoice(voice_id, juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));

    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            matrix.endingVoice(voice_id); 
            clearCurrentNote();
        }

    }

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {

        if(tailOff > 0.0)
        {
            while (--numSamples >= 0)
            {
                auto currentSample = matrix.getSample(voice_id);
                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    //TODO: Need to do what we do in StopNote (i.e. matrix.endVoice) ?
                    matrix.endingVoice(voice_id);
                    clearCurrentNote();
                    break;
                }
            }
        }
    }

private:
    Matrix&   matrix;
    const int voice_id;
    double tailOff = 0.0;
};
