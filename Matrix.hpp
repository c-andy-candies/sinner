#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <atomic>

#include "Oscillator.hpp"
#include <cstring>
#include "constants.h"

class Matrix{
public:
    Matrix();
    float getSample(int voice_id);
    void  changeParam();
    void  setSampleRate(float sampleRate);

    void  setFMIndex(uint32_t row, uint32_t column, std::atomic<float>* ptr);
    void  setAMIndex(uint32_t row, uint32_t column, std::atomic<float>* ptr);
    void  setToOutput(uint32_t osc_index, std::atomic<float>* ptr);

    void  setOscillatorVolume(uint32_t index, std::atomic<float>* ptr);
    void  setOscillatorRatio(uint32_t index, std::atomic<float>* ptr);

    void  setHarmonicRatio(uint32_t osc_index, uint32_t harmonic_index, std::atomic<float>* ptr);
    void  setHarmonicVolume(uint32_t osc_index, uint32_t harmonic_index, std::atomic<float>* ptr);

    void  startingVoice(const int voice_id, float note);
    void  endingVoice(const int voice_id);

private:
    // All the synths' oscillators.
    Oscillator			    oscillators[NUMBER_OF_OSCILLATORS];

    // AM,FM modulation indexes
    struct modulation_pair
    {
      std::atomic<float>* AM;
      std::atomic<float>* FM;
    } modulationMatrix[NUMBER_OF_OSCILLATORS][NUMBER_OF_OSCILLATORS];

    // Which oscs go to output and with which amount.
    std::atomic<float>* toOutput[NUMBER_OF_OSCILLATORS];

    // A row for each oscillator, multiple values to average in case of feeback (DX7 style: 2 last values).
    float      				   oldValuesMatrix[HISTORY_LENGTH][NUMBER_OF_OSCILLATORS];

    // A row for each oscillator.
    float					   currentValues[NUMBER_OF_OSCILLATORS];

    //TODO: Need to separate hasFeedback between AM and FM
    // Used to know if the osc is in a feedback loop and needs to average the last two values
    bool       				   hasFeedback[NUMBER_OF_OSCILLATORS][NUMBER_OF_OSCILLATORS];

    float					   getOscSample(short oscIndex, int voice_id);
    void					   checkFeedback();

    int                        updated[NUMBER_OF_OSCILLATORS];

    // Where to save currentValues
    int                        history_index;

};

#endif // MATRIX_HPP
