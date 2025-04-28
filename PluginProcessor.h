#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <tuple>
#include <vector>
#include "Matrix.hpp"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setFMIndex (int i, int j, float new_value);
    void setAMIndex (int i, int j, float new_value);
    void setToOutput (int i, float new_value);

    void setOscillatorRatio (int osc, float new_value);
    void setOscillatorVolume (int osc, float new_value);

    void setHarmonicVolume (int osc, int harmonic, float new_value);

    const std::tuple<const int, std::atomic<float>*, std::atomic<float>*, std::atomic<float>*> addEnvelopePoint (int osc, float time, float amplitude);

    void removeEnvelopePoint (int osc);

    void toggleStateEnvelopePoint (int osc);

    void moveEnvelopePoint (int osc, int point_number, float time, float amplitude);

    void changeInterpolationEnvelopeCurve (int osc);

private:
    //==============================================================================

    juce::AudioProcessorValueTreeState::ParameterLayout createMatrixParameterLayout();
    juce::AudioProcessorValueTreeState matrixParameters;

    juce::AudioProcessorValueTreeState::ParameterLayout createOscParameterLayout();
    juce::AudioProcessorValueTreeState oscillatorParameters;

    juce::Synthesiser synth;
    Matrix modulation_matrix;

    std::vector<int> inactiveEnvelopePoints;

    const int getInactiveEnvelopePoint();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
