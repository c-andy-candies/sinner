#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

struct SinnerSound   : public juce::SynthesiserSound
{
    SinnerSound() {}

    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};