#include "Harmonic.hpp"
#include "constants.h"
#include <atomic>
#include <cmath>
#include <unordered_map>

Harmonic::Harmonic(){}

void Harmonic::setRatio(std::atomic<float>* ptr)
{
    HarmonicRatio = ptr;

    //TODO: Do we need it?
    //resetDelta();
}

void Harmonic::setVolume(std::atomic<float>* ptr)
{
    HarmonicVolume = ptr;
}

//TODO: Might be useless
void Harmonic::setDelta(float osc_ratio, float note)
{
    HarmonicDelta = note * osc_ratio * HarmonicDelta;
}


//TODO:
//Does it make sense to reset it? Or should we only set it when we start a new note?
//Maybe it makes sense because we anticipate a multiplication
//Maybe instead we can keep another field that contains HarmonicRatio * DELTA and changes 
//only when we setRatio. Or only keep this new field and when we do getRatio do a division

void Harmonic::resetDelta()
{
    HarmonicDelta = *HarmonicRatio * DELTA;
}

void Harmonic::startingVoice(const int voice_id, float modulated_ratio)
{
    voicesInfos[voice_id] = {0.0f, HarmonicDelta * modulated_ratio};
}

void Harmonic::endingVoice(const int voice_id)
{
    voicesInfos.erase(voice_id);
    resetDelta();
}

float Harmonic::getSample(float freqMod, const int voice_id)
{
    //Should not be required anymore
    if(*HarmonicVolume != 0.0)
    {

        //float phase = HarmonicDelta * instant + freqMod * MAGIC_NUMBER;

        //std::cout << "phase " << phase << std::endl;

        /*
         * //no interpolation
        if (phase >= SIN_LUT_SIZE)
        {
            phase = ((int)phase) % SIN_LUT_SIZE;
        }
        */

        /*JUCE TUTORIAL BEGIN

        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float) index0;

        auto* table = wavetable.getReadPointer (0);
        auto value0 = table[index0];
        auto value1 = table[index1];

        auto currentSample = value0 + frac * (value1 - value0);

        if ((currentIndex += tableDelta) > (float) tableSize)
            currentIndex -= (float) tableSize;

        return currentSample;

        JUCE TUTORIAL END*/

        //Shouldn't the 1st sample be the zero-th index?
        //Maybe add an if to check whether freqMod != 0, otherwise we do a useless multiplication)
        //Better to have an if everytime or just waste a multiplication to 0? 
        
        //currentIndex += HarmonicDelta + freqMod * MAGIC_NUMBER;

        //instead of the lines above that require the calling of setDelta we use modulated_ratio
        //so that we can have a single matrix that calculates the input for all voices:
        //currentIndex += HarmonicDelta * modulated_ratio + freqMod * MAGIC_NUMBER;


        auto[currentIndex, actualDelta] = voicesInfos[voice_id];

        currentIndex += actualDelta + freqMod * MAGIC_NUMBER;

        //Is it necessary to cast SIN_LUT_SIZE?
        //If it is, should we have const float SIN_LUT_SIZE?
        if(currentIndex >= (float) SIN_LUT_SIZE)
        {
            currentIndex -= (float) SIN_LUT_SIZE;
        }

        voicesInfos[voice_id].currentIndex = currentIndex;

        //why unsigned? Maybe the tutorial is sure that the index
        //won't never be negative, we are not, I think
        auto index0 = (unsigned int) currentIndex;
        unsigned int index1;
        
        //cast to long or to float? And should we subtract currentIndex or index0?
        auto fractional_part_of_index = currentIndex - (long) currentIndex;

        //WHY?????? WHENNNNNN? 
        //Maybe because the FM can swing the index outside of the range
        //NEED TO CHECK ALL BOUNDARIES AND LIMIT VALUES
        if(index0 < 0)
        {
            index0 +=  SIN_LUT_SIZE;
            //WHY DIDN'T I WRITE DOCUMENTATION?????
            //WHYYYYYYYYY minus 1?
            index1 = index0 - 1;

            if(index1 < 0)
            {
                index1 = SIN_LUT_SIZE - 1;
            }

            fractional_part_of_index += 1.0f;

        }
        else
        {
            index1 = index0 + 1;

            if(index1 > SIN_LUT_SIZE - 1)
            {
                index1 = 0;
            }
        }

        /*PREVIOUS IMPL
        float fractional_part_of_phase = phase - (long)phase;

        int int_phase = (int)phase;

        if (std::abs(int_phase) >= SIN_LUT_SIZE)
        {
            int_phase = int_phase % SIN_LUT_SIZE;
        }

        int int_phase_plus_1;

        if(int_phase < 0)
        {
            int_phase =  SIN_LUT_SIZE + int_phase;
            int_phase_plus_1 = int_phase - 1;

            if(int_phase_plus_1 < 0)
            {
                int_phase_plus_1 = SIN_LUT_SIZE - 1;
            }

            fractional_part_of_phase = 1.0f + fractional_part_of_phase;

        }
        else
        {
            int_phase_plus_1 = int_phase + 1;

            if(int_phase_plus_1 > SIN_LUT_SIZE - 1)
            {
                int_phase_plus_1 = 0;
            }
        }
        */

        /*
        std::cout << "fractional_part " << fractional_part_of_phase << std::endl;

        std::cout << "int_phase " << int_phase << std::endl;
        std::cout << "int_phase_plus_1 " << int_phase_plus_1 << std::endl;
        */

        //Shouldn't it be different if index1 < index0?
        //Is it better to store the values in two variables or do the table lookup twice for index0?
        const float sample = SIN_LUT[index0] + fractional_part_of_index * (SIN_LUT[index1] - SIN_LUT[index0]);

        return *HarmonicVolume * sample;

    }

    //no point in calculating sample if gain is 0
    return 0.0;
}
