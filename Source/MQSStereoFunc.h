/*
  ==============================================================================

    MQSStereoFunc.h
    Created: 5 Mar 2021 1:38:50am
    Author:  马赛

  ==============================================================================
*/

#pragma once

#include "Oscillators.h"
#include "JuceHeader.h"

class MQSStereoFunc
{
public:

    // get pan parameter, 1 as left, 0 as right, 0.5 as center
    float getPan()
    {
        return leftGain;
    }

    // set pan parameter, 1 as left, 0 as right, 0.5 as center
    void setPan(float leftness)
    {
        leftGain = leftness;
    }

    // child class process function
    virtual float process() = 0;

    // process next left sample with pan applied
    float processL()
    {
        float processOut = process();
        // update panning level by leftGainVar
        leftGain += leftGainVar;
        return leftGain * processOut;
    }

    // process next right sample with pan applied
    float processR()
    {
        float processOut = process();
        // update panning level by leftGainVar
        leftGain += leftGainVar;
        return (1.0f - leftGain) * processOut;
    }

    // functional methods: get float midi notes in Hertz
    float getMidiInHz(float noteNumber, float frequencyOfA)
    {
        return frequencyOfA * std::pow(2.0f, (noteNumber - 69.0f) / 12.0f);
    }
    float getMidiInHz(float noteNumber)
    {
        return getMidiInHz(noteNumber, 440.0f);
    }

    // dynamic pan functions:
    // pan values random generator, input the sample length of one cycle of dynamic pan 
    void initPan(float cycleLenInSamples)
    {
        // generate random pan level
        leftGainStart = 0.2f * random.nextFloat() + (random.nextBool() ? 0.0f : 0.8f);
        // set start pan
        setPan(leftGainStart);
        // generate pan variation value per sample
        leftGainVar = (1.0f - 2.0f * leftGainStart) / cycleLenInSamples;
    }

    // reset pan
    void resetPan()
    {
        setPan(leftGainStart);
    }

private:
    juce::Random random;
    float leftGain;         // pan parameter left channel gain
    float leftGainVar;       // left variation value per sample
    float leftGainStart;     // left start value
};