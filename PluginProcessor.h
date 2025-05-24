#pragma once

#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <tuple>
#include <vector>
#include "Matrix.hpp"
#include "constants.h"

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

    float getHarmonicVolume (int osc, int harmonic);

    const std::tuple<const int, std::atomic<float>*, std::atomic<float>*, std::atomic<float>*> addEnvelopePoint (int osc, float time, float amplitude);

    void removeEnvelopePoint (int osc, int point_number);

    void moveEnvelopePoint (int osc, int point_number, float time, float amplitude);

    void changeInterpolationEnvelopeCurve (int osc);

    const std::tuple<const int, std::atomic<float>*, std::atomic<float>*> addEnvelopeRange (int osc, float time);

    void removeEnvelopeRange (int osc, int range_number);

    void moveEnvelopeRange (int osc, int range_number, float time);

    void changeNumberOfRepsEnvelopeRange(int osc, int range_number, int reps);

private:
    //==============================================================================

    juce::AudioProcessorValueTreeState::ParameterLayout createMatrixParameterLayout();
    juce::AudioProcessorValueTreeState matrixParameters;

    juce::AudioProcessorValueTreeState::ParameterLayout createOscParameterLayout(int osc);
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState>, NUMBER_OF_OSCILLATORS> oscillatorParameters;

    juce::Synthesiser synth;
    Matrix modulation_matrix;

    //TODO: might be possible to move into OscillatorView
    std::array<std::vector<int>, NUMBER_OF_OSCILLATORS> inactiveEnvelopePoints;
    const int getInactiveEnvelopePoint(int osc);

    //TODO: might be possible to move into OscillatorView
    std::array<std::vector<int>, NUMBER_OF_OSCILLATORS> inactiveEnvelopeRanges;
    const int getInactiveEnvelopeRange(int osc);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
