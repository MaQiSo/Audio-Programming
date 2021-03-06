/*
  ==============================================================================

    MQSInFall.h
    Created: 4 Mar 2021 2:19:33am
    Author:  马赛

    MQS In Fall

  ==============================================================================
*/

#pragma once

#include "MQSSynth.h"

using namespace juce;
class MQSInFall : public MQSStereoFunc
{
public:
    // set sample rate
    void setSampleRate(float _sampleRate)
    {
        sampleRate = _sampleRate;
        volumeLfo.setSampleRate(sampleRate);
        genParam();
    }

    // set filter parameters
    void setFiltrParam(float _note)
    {
        float freq = getMidiInHz(_note);
        fallFilter.setCoefficients(IIRCoefficients::makeBandPass(sampleRate, freq, filterQ));
    }

    // generate random parameters
    void genParam()
    {
        // generate random volume LFO frequency
        float volumeModFreq = random.nextFloat() * 0.12f + 0.06f;
        // sample length of one cycle of volume LFO
        float volModSample = sampleRate / volumeModFreq;
        // set frequency
        volumeLfo.setFrequency(volumeModFreq);

        note = round(random.nextFloat() * 38.0f) + 36.0f; // random note
        noteVar = random.nextFloat() * 36.0f / volModSample; // random noteVar
                // generate random filterQ
        filterQ = random.nextFloat() * 8.0f + 8.0f;

        // initialise filter
        setFiltrParam(note);
        fallFilter.reset();

        // initialise random pan level
        initPan(volModSample);
    }

    // synth processor
    float process() override
    {
        // generate next noise output
        float noise = random.nextFloat() - 0.5f;
        noise = fallFilter.processSingleSampleRaw(noise);
        noise *= 8.0f * volumeLfo.process(); // apply output amplitude coefficients

        // update note
        note -= noteVar;
        setFiltrParam(note);

        // check if volumeLfo just completed one cycle
        if (volumeLfo.ifCycleOver())
        {
            // start new sound
            genParam();
        }
        return noise;
    }
private:
    Sin2Osc volumeLfo; // volume LFO
    IIRFilter fallFilter;  // filter for creating fall effects

    float filterQ{ 10.0f };     // filter parameter: Q
    float note;        // center note
    float noteVar;     // note variation per sample

    float sampleRate{ 44100.0f };   // sample rate
    Random random;
};