#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SinnerSound.cpp"
#include "SinnerVoice.cpp"
#include "constants.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include <array>
#include <memory>
#include <string>
#include <tuple>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
     , matrixParameters (*this, nullptr, juce::Identifier ("MatrixParameters"), createMatrixParameterLayout()) 
{

    for (auto i = 0; i < NUMBER_OF_VOICES; ++i)
    {
        synth.addVoice (new SinnerVoice(modulation_matrix, i));
    }

    synth.addSound (new SinnerSound());

    //Here we store the pointers to the parameters in the classes where we need them to perform calculations
    for(auto i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        const std::string oscillator_number = std::to_string(i);

        modulation_matrix.setToOutput(i, matrixParameters.getRawParameterValue("toOutput_" + oscillator_number));

        for(auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
        {
            const std::string oscillator2_number = std::to_string(j);
            modulation_matrix.setAMIndex(i, j,matrixParameters.getRawParameterValue("AM_index_(" + oscillator_number + "," +  oscillator2_number + ")"));
            modulation_matrix.setFMIndex(i, j,matrixParameters.getRawParameterValue("FM_index_(" + oscillator_number + "," +  oscillator2_number + ")"));
        }

        modulation_matrix.setOscillatorRatio(i, matrixParameters.getRawParameterValue("osc" + oscillator_number + "_ratio"));
        modulation_matrix.setOscillatorVolume(i, matrixParameters.getRawParameterValue("osc" + oscillator_number + "_volume"));

        for(auto j = 0; j < NUMBER_OF_HARMONICS; ++j)
        {
            const std::string harmonic_number = std::to_string(j);
            modulation_matrix.setHarmonicRatio(i, j, matrixParameters.getRawParameterValue("harmonic" + harmonic_number + "_ratio_of_osc_" + oscillator_number));
            modulation_matrix.setHarmonicVolume(i, j, matrixParameters.getRawParameterValue("harmonic" + harmonic_number + "_volume_of_osc_" + oscillator_number));
        }

        oscillatorParameters[i] = std::make_unique<juce::AudioProcessorValueTreeState>(
            *this, nullptr, 
            juce::Identifier("Oscillator" + std::to_string(i) + "Parameters"), 
            createOscParameterLayout(i));

        for(auto env = 0; env < NUMBER_OF_ENVELOPE_POINTS; ++env)
        {
            inactiveEnvelopePoints[i].push_back(env);
        }
        
    } 

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    synth.setCurrentPlaybackSampleRate(sampleRate);

    juce::ignoreUnused (samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;

    //needed because renderNextBlock adds to the content of the buffer
    buffer.clear();

    //TODO: not sure we should start from 0 but don't know how to get firstSampleToFillIndex
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    auto* editor = new AudioPluginAudioProcessorEditor(*this);
    
    return editor;
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

void AudioPluginAudioProcessor::setFMIndex (int i, int j, float new_value)
{
    matrixParameters.getParameter("FM_index_(" + std::to_string(i) + "," +  std::to_string(j) + ")")->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::setAMIndex (int i, int j, float new_value)
{
    matrixParameters.getParameter("AM_index_(" + std::to_string(i) + "," +  std::to_string(j) + ")")->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::setToOutput (int i, float new_value)
{
    matrixParameters.getParameter("toOutput_" + std::to_string(i))->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::setOscillatorRatio (int osc, float new_value)
{
    //TODO: need to call it on the correct osc
    oscillatorParameters[osc]->getParameter("osc_" + std::to_string(osc) + "_ratio")->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::setOscillatorVolume (int osc, float new_value)
{
    //TODO: need to call it on the correct osc
    oscillatorParameters[osc]->getParameter("osc_" + std::to_string(osc) + "_volume")->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::setHarmonicVolume (int osc, int harmonic, float new_value)
{
    //TODO: need to call it on the correct osc
    oscillatorParameters[osc]->getParameter("harmonic" + std::to_string(harmonic) + "_ratio_of_osc_0")->setValueNotifyingHost(new_value);
}

void AudioPluginAudioProcessor::moveEnvelopePoint (int osc, int point_number, float time_new_value, float amplitude_new_value)
{

    auto oscillator_number = std::to_string(osc);
    auto envelope_point_number = std::to_string(point_number);

    oscillatorParameters[osc]->getParameter("env_point" + envelope_point_number + "_time_osc_" + oscillator_number)->setValueNotifyingHost(time_new_value);
    oscillatorParameters[osc]->getParameter("env_point" + envelope_point_number + "_amplitude_osc_" + oscillator_number)->setValueNotifyingHost(amplitude_new_value);

}

const int AudioPluginAudioProcessor::getInactiveEnvelopePoint(int osc)
{
    //TODO: need to do it for the correct osc

    auto inactivePoint = inactiveEnvelopePoints[osc].back();
    inactiveEnvelopePoints[osc].pop_back();
    return inactivePoint;

}

const std::tuple<const int, std::atomic<float>*, std::atomic<float>*, std::atomic<float>*> AudioPluginAudioProcessor::addEnvelopePoint (int osc, float time_new_value, float amplitude_new_value)
{

    auto oscillator_number = std::to_string(osc);
    
    auto point_number = getInactiveEnvelopePoint(osc);
    auto envelope_point_number = std::to_string(point_number);

    oscillatorParameters[osc]->getParameter("env_point" + envelope_point_number + "_time_osc_" + oscillator_number)->setValueNotifyingHost(time_new_value);
    oscillatorParameters[osc]->getParameter("env_point" + envelope_point_number + "_amplitude_osc_" + oscillator_number)->setValueNotifyingHost(amplitude_new_value);

    auto time_ptr = oscillatorParameters[osc]->getRawParameterValue     ("env_point" + envelope_point_number + "_time_osc_" + oscillator_number);
    auto amplitude_ptr = oscillatorParameters[osc]->getRawParameterValue("env_point" + envelope_point_number + "_amplitude_osc_" + oscillator_number);
    auto interpolation_ptr = oscillatorParameters[osc]->getRawParameterValue("env_point" + envelope_point_number + "_interp_osc_" + oscillator_number);
    
    return std::make_tuple(point_number, time_ptr, amplitude_ptr, interpolation_ptr); 
}

void AudioPluginAudioProcessor::removeEnvelopePoint(int osc, int point_number)
{
    //TODO: need to do it for the correct osc
    inactiveEnvelopePoints[osc].push_back(point_number);
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createMatrixParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
 
    //Adding Matrix parameters
    for (auto i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        auto oscillator_number = std::to_string(i);
        
        //toOutput
        layout.add (std::make_unique<juce::AudioParameterFloat> ("toOutput_" + oscillator_number, "Output gain of Osc " + oscillator_number, juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f));
        
        for(auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
        {
            auto harmonic_number = std::to_string(j);

            //FM indexes
            layout.add(std::make_unique<juce::AudioParameterFloat>("FM_index_(" + oscillator_number + "," +  harmonic_number + ")", "FM Index (" + oscillator_number + "," + harmonic_number + ")", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
            //AM indexes
            layout.add(std::make_unique<juce::AudioParameterFloat>("AM_index_(" + oscillator_number + "," +  harmonic_number + ")", "AM Index (" + oscillator_number + "," + harmonic_number + ")", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
        }
         
    }
    return layout;
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createOscParameterLayout(int osc)
{

    //TODO: Research if it's better to have a different parameter layout for each oscillator or just one
    //maybe separation is better for the undo/redo possibilities (e.g. while working on a oscillator i am in its page and if I undo I want to undo here not somewhere I can't see i.e. another place)
    //Maybe we can also generate all the parametersLayouts all in one method since anyway we have to iterate through the whole "matrix"

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto oscillator_number = std::to_string(osc);

    //Oscillator Ratio
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc_" + oscillator_number + "_ratio", "Oscillator " + oscillator_number + " Ratio", juce::NormalisableRange<float>(0.0f,1.0f),1.0f)); 
    //Oscillator Volume
    layout.add(std::make_unique<juce::AudioParameterFloat>("osc_" + oscillator_number + "_volume", "Oscillator " + oscillator_number + " Volume", juce::NormalisableRange<float>(0.0f,1.0f),1.0f));

    for(auto j = 0; j < NUMBER_OF_HARMONICS; ++j)
    {
        auto harmonic_number = std::to_string(j);

        //Harmonic Ratio
        layout.add(std::make_unique<juce::AudioParameterFloat>("harmonic" + harmonic_number + "_ratio_of_osc_" + oscillator_number, "Harmonic " + harmonic_number + "'s Ratio of Oscillator " + oscillator_number, juce::NormalisableRange<float>(0.0f,1.0f), 0.0f));
        //Harmonic Volume
        layout.add(std::make_unique<juce::AudioParameterFloat>("harmonic" + harmonic_number + "_volume_of_osc_" + oscillator_number, "Harmonic " + harmonic_number + "'s Volume of Oscillator " + oscillator_number, juce::NormalisableRange<float>(0.0f,1.0f), 0.0f));
    }

    for(auto i = 0; i < NUMBER_OF_ENVELOPE_POINTS; ++i)
    {
        auto point_number = std::to_string(i);

        layout.add(std::make_unique<juce::AudioParameterFloat>("env_point" + point_number + "_time_osc_" + oscillator_number, "Envelope Point " + point_number + "'s Time of Oscillator " + oscillator_number, juce::NormalisableRange<float>(0.0f,1.0f), 0.0f));    

        layout.add(std::make_unique<juce::AudioParameterFloat>("env_point" + point_number + "_amplitude_osc_" + oscillator_number, "Envelope Point " + point_number + "'s Amplitude of Oscillator " + oscillator_number, juce::NormalisableRange<float>(0.0f,1.0f), 0.0f));    

        //Type of interpolation of the curve which ENDS in this point
        layout.add(std::make_unique<juce::AudioParameterFloat>("env_curve" + point_number + "_interp_osc_" + oscillator_number, "Envelope Curve " + point_number + "'s Interpolation of Oscillator " + oscillator_number, juce::NormalisableRange<float>(0.0f,1.0f), 0.0f));    

    }

    return layout;

}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

