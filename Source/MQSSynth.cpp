/*
  ==============================================================================

    MQSSynth.cpp
    Created: 28 Feb 2021 1:13:45am
    Author:  马赛

  ==============================================================================
*/

#include "MQSSynth.h"

void MQSSynth::setSampleRate(float _sampleRate)
{
    sampleRate = _sampleRate;
    osc1.setSampleRate(sampleRate);
    osc2.setSampleRate(sampleRate);
    modLfo.setSampleRate(sampleRate);
    volumeLfo.setSampleRate(sampleRate);

    genParam();
}

void MQSSynth::setNote(float note)
{
    float freq1 = getMidiInHz(note);
    float freq2 = getMidiInHz(note - interval);

    osc1.setFrequency(freq1);
    osc2.setFrequency(freq2);
}

void MQSSynth::setModLFO(float freq)
{
    modLfo.setFrequency(freq);
}

void MQSSynth::setVolLFO(float freq)
{
    volumeLfo.setFrequency(freq);
}

void MQSSynth::setInterval(float _intrvl)
{
    interval = _intrvl;
}

void MQSSynth::setIntrvlbyType(int _intrvl)
{
    setIntrvlbyType(static_cast<IntrvlName>(_intrvl));
}

void MQSSynth::setIntrvlbyType(IntrvlName _intrvl)
{
    switch (_intrvl)
    {
    case minor2:
        interval = 1;
        break;
    case major2:
        interval = 2;
        break;
    case minor3:
        interval = 3;
        break;
    case major3:
        interval = 4;
        break;
    case perfect4:
        interval = 5;
        break;
    case perfect5:
        interval = 7;
        break;
    case minor6:
        interval = 8;
        break;
    case major6:
        interval = 9;
        break;
    }
    setNote(note);
}

void MQSSynth::genParam()
{
    // randomize all the coefficients
    juce::Random random;
    modLfo.setFrequency(random.nextFloat() * 2.0f); // random mod LFO freq
    
    // generate random volume LFO frequency
    float volumeModFreq = random.nextFloat() * 0.12f + 0.016f;
    // sample length of one cycle of volume LFO
    float volModSample = sampleRate / volumeModFreq;
    // set random volume LFO freq
    volumeLfo.setFrequency(volumeModFreq);

    // initialise random pan level
    initPan(volModSample);

    // generate random note parameters
    note = round(random.nextFloat() * 36.0f) + 36.0f; // random note
    noteVar = ((random.nextBool()) ? 1.0f : -1.0f) / volModSample;
    setNote(note); // set note

    repeatT = random.nextInt() % 5 + 2;  // random repeat term
}

bool MQSSynth::isThrough()
{
    return (volume < 1e-2f);
}

float MQSSynth::process()
{
    // generate next osc output
    float oscOut = 0.5f * (osc1.process() + osc2.process());
    oscOut *= modLfo.process();
    volume = volumeLfo.process();
    oscOut *= 2.0f * volume;

    // implement gliss
    note += noteVar;
    setNote(note);

    // check if volumeLfo just completed one cycle
    if (volumeLfo.ifCycleOver())
    {
        // reset gliss direction
        noteVar *= -1.0f;
        // add up repeat time
        currentRepT++;
        // reset panning
        resetPan();
        // check if the same parameters have repeated enough times
        if (currentRepT > repeatT)
        {
            // get new sound
            genParam();
            // reset repeat time
            currentRepT = 0;
        }
    }
    return oscOut;
}