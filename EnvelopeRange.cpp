#pragma once

#include <atomic>

class EnvelopeRange
{

public:

    EnvelopeRange() : rangeNumber(-1) {}

    EnvelopeRange(int number, std::atomic<float>* time_ptr, std::atomic<float>* reps_ptr)
        : rangeNumber(number), time(time_ptr), numberOfReps(reps_ptr)
        {}


    bool operator<(const EnvelopeRange& other) const
    {
        return *time < *other.time;
    }

    int getRangeNumber() const { return rangeNumber; }
    float getTime() const { return *time; }
    int getNumberOfReps() const { return (int)*numberOfReps; } 

private:
    int rangeNumber;
    std::atomic<float>* time;
    std::atomic<float>* numberOfReps;

};
