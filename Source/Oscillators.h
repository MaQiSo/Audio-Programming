/*
  ==============================================================================

    Oscillators.h
    Created: 27 Feb 2021 3:18:41pm
    Author:  马赛

  ==============================================================================
*/

#pragma once

#include <cmath>

// PARENT phasor class
class Phasor
{
public:

    // Parent oscillator class: 
    // -- handles phase
    // -- handles setters and getters for frequency and samplerate

    /// update the phase and output for the next sample from the oscillator
    float process()
    {
        phase += phaseDelta;

        if (phase > 1.0f)
        {
            phase -= 1.0f;
            cycleOver = true;
        }
        else
        {
            cycleOver = false;
        } 
        return output(phase);
    }
    
    virtual float output(float p)
    {
        return p;
    }

    // set frequency
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }

    // set sample rate
    void setSampleRate(float SR)
    {
        // update frequency with new sample rate
        phaseDelta *= sampleRate;
        sampleRate = SR;
        phaseDelta /= sampleRate;
    }

    // return true if a cycle just over
    bool ifCycleOver()
    {
        return cycleOver;
    }

private:
    float frequency{ 440.0f };
    float sampleRate{ 44100.0f };
    float phase{ 0.0f };
    bool cycleOver{ false };
    float phaseDelta;
};

// ================================================

// CHILD Class
class TriOsc : public Phasor
{
    float output(float p) override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};

// CHILD Class
class SinOsc : public Phasor
{
    float output(float p) override
    {
        return std::sin(p * 2.0f * 3.14159f);
    }
};

// CHILD Class
class SquareOsc : public Phasor
{
    float output(float p) override
    {
        float outVal{ 0.5f };
        if (p > pulseWidth)
            outVal = -0.5f;
        return outVal;
    }
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth;
};

// CHILD Class
// for amplitude Mod LFO
class Sin12Osc : public Phasor
{
    float output(float p) override
    {
        return pow(std::sin(p * 3.14159f),12);
    }
};

// CHILD Class
// for amplitude Mod LFO
class Sin2Osc : public Phasor
{
    float output(float p) override
    {
        return pow(std::sin(p * 3.14159f), 2);
    }
};
