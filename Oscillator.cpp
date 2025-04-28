#include "Oscillator.hpp"
#include "constants.h"

#include <atomic>
#include <iostream>
#include <string>

Oscillator::Oscillator()
{
    for (auto i = 0; i < NUMBER_OF_HARMONICS; ++i)
    {
        Harmonic harmonic;
        UnusedHarmonics[i] = harmonic;
    }
}

void Oscillator::setRatio(std::atomic<float>* ptr)
{
    OscillatorRatio = ptr;
}

void Oscillator::setVolume(std::atomic<float>* ptr)
{
    OscillatorVolume = ptr;
}

float Oscillator::getSample(float ampModulator, float freqModulator, int voice_id)
{
    float sample = 0.0;

    for(auto harmonic : UsedHarmonics)
    {
        sample += harmonic.second.getSample(freqModulator, voice_id);
    }

    //TODO: Add note_ratio to the computation of AM, somehow, gotta learn about it
    return *OscillatorVolume * ampModulator * sample;
}

void Oscillator::setHarmonicVolume(short harmonic_index, std::atomic<float>* ptr)
{
    //Since this is called at creation, they should all be in unused
    UnusedHarmonics[harmonic_index].setVolume(ptr);
}

//Temporary debugging function
void Oscillator::printOscStatus()
{
    /*
    for(int i = 0; i < 10; i++)
    {
        std::cout << "(volume, ratio) harm" << i << " " << UsedHarmonics[i].getVolume() << " " << UnusedHarmonics[i].getRatio() << std::endl;
    }
    */
}

void Oscillator::setHarmonicRatio(short harmonic_index, std::atomic<float>* ptr)
{
    UnusedHarmonics[harmonic_index].setRatio(ptr);
}

void Oscillator::startingVoice(const int voice_id, float note)
{
    for(auto harmonic : UsedHarmonics)
    {
        harmonic.second.startingVoice(voice_id, note);
    }
}

void Oscillator::endingVoice(const int voice_id)
{
    for(auto harmonic : UsedHarmonics)
    {
        harmonic.second.endingVoice(voice_id);
    }
}