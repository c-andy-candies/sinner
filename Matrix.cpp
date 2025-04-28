#include "Matrix.hpp"
#include "CircularIndex.h"

#include <cstring>

Matrix::Matrix()
{
    for(int i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        Oscillator osc;
        oscillators[i] = osc;
    }
}

void Matrix::setFMIndex(uint32_t row, uint32_t column, std::atomic<float>* ptr)
{
   modulationMatrix[row][column].FM = ptr;
}

void Matrix::setAMIndex(uint32_t row, uint32_t column, std::atomic<float>* ptr)
{
   modulationMatrix[row][column].AM = ptr;
}

void Matrix::setToOutput(uint32_t osc_index, std::atomic<float>* ptr)
{
    toOutput[osc_index] = ptr;
}

void Matrix::setOscillatorVolume(uint32_t index, std::atomic<float>* ptr)
{
    oscillators[index].setVolume(ptr);
}

void Matrix::setOscillatorRatio(uint32_t index, std::atomic<float>* ptr)
{
    oscillators[index].setRatio(ptr);
}

void Matrix::setHarmonicRatio(uint32_t osc_index, uint32_t harmonic_index, std::atomic<float>* ptr)
{
    oscillators[osc_index].setHarmonicRatio(harmonic_index, ptr);
}

void Matrix::setHarmonicVolume(uint32_t osc_index, uint32_t harmonic_index, std::atomic<float>* ptr)
{
    oscillators[osc_index].setHarmonicVolume(harmonic_index, ptr);
}

void Matrix::setSampleRate(float sampleRate)
{
    return;
}

void Matrix::startingVoice(const int voice_id, float note)
{
    for(auto i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        oscillators[i].startingVoice(voice_id, note);
    }
}

void Matrix::endingVoice(const int voice_id)
{
    for(auto i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        oscillators[i].endingVoice(voice_id);
    }
}

//TODO: AM is not implemented, gotta learn about its feedback and everything else
float Matrix::getSample(int voice_id)
{
    float result = 0.0;

    for(int i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        if(*toOutput[i] != 0.0)
        {
            result += getOscSample(i, voice_id);
        }
    }

    //Here we update oldValues by deleting oldest ones's slice and making currentValues the newest oldValues
    memcpy(oldValuesMatrix[history_index], currentValues, sizeof(NUMBER_OF_OSCILLATORS));
    history_index = next(history_index);

    //TODO: Maybe set it to a real impossible value like MAXFLOAT
    //maybe it's useless now that we replace them
    memset(currentValues, 0.0f, sizeof (currentValues));

    //TODO: 11 is arbitrary, would be better to keep 0 as default and the oscIndexes start from 1 (need to check it doesn't break anything though)
    memset(updated, 11, sizeof(int));

    return result;
}

float Matrix::getOscSample(short oscIndex, int voice_id)
{
    float FM_modulation_value = 0.0;
    float AM_modulation_value = 1.0;

    for(int i = 0; i < NUMBER_OF_OSCILLATORS; ++i)
    {
        float FM_modulation_index = *modulationMatrix[i][oscIndex].FM;
        if(FM_modulation_index != 0.0)
        {
            //TODO:
            //if(hasFeedback[i][oscIndex])
            if(0)
            {
                //[1]
                //Need to understand when to update oldValues
                FM_modulation_value += FM_modulation_index * oldValuesMatrix[i][0]; // or modulationValue *= modulationIndex * ((oldValuesMatrix[i][0] + oldValuesMatrix[i][1]) / 2.0);
                if(updated[oscIndex] == 11)
                {
                    //[2]
                    updated[i] = oscIndex;
                    //gets called just to update the currentValue / oldValues
                    currentValues[i] = getOscSample(i, voice_id);
                }
                else if(updated[oscIndex] != i)
                {
                    //[3]
                    updated[i] = updated[oscIndex];
                    //gets called just to update the currentValue / oldValues
                    currentValues[i] = getOscSample(i, voice_id);
                }
                else 
                {
                    //[4]
                    //should probably do it for every item that has updated[oscIndex] == i
                    //updated[oscIndex] = 11;
                    for(auto j = 0; j < NUMBER_OF_OSCILLATORS; ++j)
                    {
                        if(updated[j] == i)
                        {
                            updated[j] = 11;
                        }
                    }
                }
                
            }
            else
            {
                //[5]
                //TODO: are we sure?
                //value should always be between -1 and 1, hence the check
                //modulator VALUE NOT YET UPDATED-> NEED TO GET THE CURRENT VALUE of this modulator
                //if(currentValues[i] > 1.0f)
                if (updated[i] != i)
                {
                    //[6] update modulator current value 
                    currentValues[i] = getOscSample(i, voice_id);
                    FM_modulation_value += FM_modulation_index * currentValues[i];
                    //std::cout << "osc " << i << " value " << FM_modulation_value << std::endl;
                }
                //modulator VALUE IS ALREADY UPDATED
                else
                {
                    //[7]
                    FM_modulation_value += FM_modulation_index * currentValues[i];
                    //std::cout << "osc " << i << " value " << FM_modulation_value << std::endl;
                }
            }
        }

        float AM_modulation_index = *modulationMatrix[i][oscIndex].AM;
        /*
        if(AM_modulation_index > 0.0)
        {
            //hasFeedback
            if(0)
            {
                //TODO: need to learn about AM feedback
            }
            else
            {
                if(currentValues[i] > 1.0f)
                {
                    currentValues[i] = getOscSample(i, voice_id);
                    AM_modulation_value *= AM_modulation_index * currentValues[i];
                    //std::cout << "osc " << i << " value " << FM_modulation_value << std::endl;
                }
                else
                {
                    AM_modulation_value *= AM_modulation_index * currentValues[i];
                    //std::cout << "osc " << i << " value " << FM_modulation_value << std::endl;
                }
            }

        }
        */
    }

    //[8]
    currentValues[oscIndex] = oscillators[oscIndex].getSample(AM_modulation_value, FM_modulation_value, voice_id);
    updated[oscIndex] = oscIndex;
    return currentValues[oscIndex];
}

void Matrix::checkFeedback()
{

    //TODO: research what is the best data structure and algorithm to check circular dependencies.
    //This is the method to set the feedback matrix, to be called when a modulation parameter changes.
    //maybe have parameters saying which modulation setting has changed so that I do not
    //have to check the whole matrix.

}
