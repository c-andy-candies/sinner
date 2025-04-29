#pragma once

#include <set>
#include <unordered_map>

#include "EnvelopePoint.cpp"
#include "Harmonic.hpp"

class OscillatorView
{
public:

    OscillatorView() 
    {
        oscNumber = -1;
    }

    OscillatorView(int number) : oscNumber(number)
    {

    }

    auto getOscNumber() const
    {
        return oscNumber;
    }

    auto getEnvelopePointsSize() const
    {
        return envelopePoints.size();
    }

    std::set<EnvelopePoint> getEnvelopePoints() const
    {
        return envelopePoints;
    }

    std::set<EnvelopePoint>::iterator getEnvelopePointsBegin() const
    {
        return envelopePoints.begin();
    }

    std::set<EnvelopePoint>::iterator getEnvelopePointsEnd() const
    {
        return envelopePoints.end();
    }

    bool isEnvelopePointsEmpty() const
    {
        return envelopePoints.empty();
    }

    void addEnvelopePoint(EnvelopePoint new_point)
    {
        envelopePoints.insert(new_point);
    }

    void removeEnvelopePoint(std::set<EnvelopePoint>::iterator index)
    {
        envelopePoints.erase(index);
    }

private:

    int oscNumber;

    std::unordered_map<int, Harmonic> harmonics;
    
    std::set<EnvelopePoint> envelopePoints;

};