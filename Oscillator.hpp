#pragma once
#include "Harmonic.hpp"
#include <atomic>
#include <unordered_map>
#include <juce_audio_processors/juce_audio_processors.h>

class Oscillator {
public:
    Oscillator();

    void setRatio(std::atomic<float>* ptr);
    void setVolume(std::atomic<float>* ptr);

    float getSample(float ampModulator, float freqModulator, int voice_id);

    void  setHarmonicRatio(short harmonic_index, std::atomic<float>* ptr);
    void  setHarmonicVolume(short harmonic_index, std::atomic<float>* ptr);

    void  startingVoice(const int voice_id, float note);
    void  endingVoice(const int voice_id);

private:
    std::atomic<float>* OscillatorVolume;
    std::atomic<float>* OscillatorRatio;

    std::unordered_map<int, Harmonic> UsedHarmonics;
    std::unordered_map<int, Harmonic> UnusedHarmonics;

    void printOscStatus();

};

