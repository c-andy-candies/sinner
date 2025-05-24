#pragma once

#include <atomic>

class EnvelopePoint
{

public:

    EnvelopePoint() : pointNumber(-1) {}

    EnvelopePoint(float val) : pointNumber(-1)
    {
        time = new std::atomic<float>(val);
    }

    EnvelopePoint(int number, std::atomic<float>* time_ptr, std::atomic<float>* amplitude_ptr, std::atomic<float>* interpolation_ptr)
        : pointNumber(number), time(time_ptr), amplitude(amplitude_ptr), interpolation(interpolation_ptr) 
        {
        }


    bool operator<(const EnvelopePoint& other) const
    {
        return *time < *other.time;
    }

    int getPointNumber() const { return pointNumber; }
    float getTime() const { return *time; }
    float getAmplitude() const { return *amplitude; }
    float getInterpolation() const { return *interpolation; }

private:
    int pointNumber;
    std::atomic<float>* time;
    std::atomic<float>* amplitude;
    std::atomic<float>* interpolation;

};
