#pragma once

#include <atomic>
#include <cstdint>
#include <unordered_map>

#include <juce_audio_processors/juce_audio_processors.h>

class Harmonic {

public:

    Harmonic();
    Harmonic(float ratio);
    Harmonic(float ratio, float volume, int osc_index, int harmonic_index);

    float* getSamples(uint32_t frames);
    float  getSample(float freqMod, int voice_id);

    void   setRatio(std::atomic<float>* ptr);
    void   setVolume(std::atomic<float>* ptr);

    void   setDelta(float osc_ratio, float note);
    void   resetDelta();

    void   startingVoice(const int voice_id, float modulated_ratio);
    void   endingVoice(const int voice_id);

private:
    std::atomic<float>*  HarmonicRatio;
    std::atomic<float>*  HarmonicVolume;

    int    HarmonicDelta;

    //United to only perform the lookup once: voice_id -> <currentVoiceIndex, voiceNote>
    struct voice_info
    {
        float currentIndex;
        float actualDelta;
    };

    std::unordered_map<int, voice_info> voicesInfos;
};

