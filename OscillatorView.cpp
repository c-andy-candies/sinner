#pragma once

#include <set>
#include <unordered_map>

#include "EnvelopePoint.cpp"
#include "EnvelopeRange.cpp"
#include "Harmonic.hpp"

class OscillatorView
{
public:

    OscillatorView() : oscNumber(-1) {}

    OscillatorView(int number) : oscNumber(number) {}

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

    auto getEnvelopeRangesSize() const
    {
        return envelopeRanges.size();
    }

    std::set<EnvelopeRange> getEnvelopeRanges() const
    {
        return envelopeRanges;
    }

    std::set<EnvelopeRange>::iterator getEnvelopeRangesBegin() const
    {
        return envelopeRanges.begin();
    }

    std::set<EnvelopeRange>::iterator getEnvelopeRangesEnd() const
    {
        return envelopeRanges.end();
    }

    bool isEnvelopeRangesEmpty() const
    {
        return envelopePoints.empty();
    }

    void addEnvelopeRange(EnvelopeRange new_range)
    {
        envelopeRanges.insert(new_range);
    }

    void removeEnvelopeRange(std::set<EnvelopeRange>::iterator index)
    {
        envelopeRanges.erase(index);
    }

    float findClosestPointValue(float target) 
    {

        auto it = envelopePoints.lower_bound(EnvelopePoint(target));

        float closestValue = 0.0f;
        float closestDistance = std::numeric_limits<float>::max();

        if (it != envelopePoints.end())
        {

            //Check the point found by lower_bound i.e. the smallest point which is not lower than target
            float point_time = it->getTime();
            float distance = std::abs(point_time - target);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestValue = point_time;
            }
            
            //Check the point before the found lower_bound
            if (it != envelopePoints.begin()) {
                --it;
                point_time = it->getTime();
                distance = std::abs(point_time - target);
                if (distance < closestDistance) {
                    closestValue = point_time;
                }
            }
        }
        else
        {
            if (!envelopePoints.empty())
            {
                closestValue = envelopePoints.begin()->getTime();
            }
        
        }

        return closestValue;
    }

    float getRangeMinTime(float current_range)
    {
        return envelopePoints.upper_bound(EnvelopePoint(current_range))->getTime();
    }

    float getRangeMaxTime(float current_range)
    {
        auto lb = envelopePoints.lower_bound(EnvelopePoint(current_range));
        return (--lb)->getTime();
    }

    float getFirstPointTime()
    {
        return envelopePoints.begin()->getTime();
    }

    float getLastPointTime()
    {
        return (--envelopePoints.end())->getTime();
    }

private:

    int oscNumber;

    std::unordered_map<int, Harmonic> harmonics;
    
    std::set<EnvelopePoint> envelopePoints;
    
    std::set<EnvelopeRange> envelopeRanges;

};