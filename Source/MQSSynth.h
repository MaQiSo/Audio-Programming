/*
  ==============================================================================

    MQSSynth.h
    Created: 27 Feb 2021 10:50:37pm
    Author:  马赛

  ==============================================================================
*/

#pragma once

#include "MQSStereoFunc.h"
class MQSSynth : public MQSStereoFunc
{
public:
    // interval type definition
    enum IntrvlName
    {
        minor2, major2, minor3, major3, perfect4, perfect5, minor6, major6, INTRVL_NR_ITEMS
    };

    // set samplerate and take the random parameters
    void setSampleRate(float sampleRate);
    
    // set osc frequency by note name
    void setNote(float note);

    // set Modulation LFO frequency
    void setModLFO(float freq);

    // set Volume LFO frequency
    void setVolLFO(float freq);

    // set interval between 2 osc in semitones
    void setInterval(float _intrvl);

    // set interval by interval type
    void setIntrvlbyType(int _intrvl);
    void setIntrvlbyType(IntrvlName _intrvl);

    // generate Parameters
    void genParam();

    // check if the Synth is during its through period
    bool isThrough();

    // process next sample
    float process() override;

private:

    float interval{ 5.0f };  // interval semitones

    SinOsc osc1;
    SinOsc osc2;
    SinOsc modLfo;
    Sin12Osc volumeLfo;      // broad LFO creating absence to this synth

    float note;              // note number
    float noteVar;           // note number variation per sample

    float volume{ 0.0f };    // used to check if the Synth is during its through period

    float sampleRate{ 44100.0f };   // sample rate

    int repeatT{ 1 };        // same parameters repeat terms
    int currentRepT{ 0 };    // current repeat terms
};

// ===============================================================