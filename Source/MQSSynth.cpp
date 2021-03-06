/*
  ==============================================================================

    MQSSynth.cpp
    Created: 28 Feb 2021 1:13:45am
    Author:  马赛

  ==============================================================================
*/

#include "MQSSynth.h"

float MQSSynth::getSemiByIntrvl(IntrvlName name)
{
    switch (name)
    {
    case minor2:
        return 1.0f;
    case major2:
        return 2.0f;
    case minor3:
        return 3.0f;
    case major3:
        return 4.0f;
    case perfect4:
        return 5.0f;
    case perfect5:
        return 7.0f;
    case minor6:
        return 8.0f;
    case major6:
        return 9.0f;
    }
    return 5.0f; // handel unexpected
}

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

void MQSSynth::setSeedNote(float seed)
{
    // ensure seed note is between the range of the base octave
    seedNote = fmod(abs(seed), 12);
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
    nextIntrvl = getSemiByIntrvl(_intrvl);
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
    interval = nextIntrvl; // update new interval value
    note = round(seedNote + interval * round(random.nextFloat() * 36.0f / interval) + 24.0f); // random note
    // note = round(random.nextFloat() * 36.0f) + 36.0f; 
    noteVar = (random.nextBool() ? 1.0f : -1.0f) / volModSample;
    note += ((noteVar > 0) ? -0.5f : 0.5f);
    setNote(note); // set note

    repeatT = random.nextInt() % 5 + 2;  // random repeat term

    // reset repeat time
    currentRepT = 0;
}

bool MQSSynth::isThrough()
{
    // return true if the VolumeLfo(envelope) is during its thourgh time
    return (volume < 2e-2f);
}

float MQSSynth::process()
{
    // generate next osc output
    float oscOut = 0.5f * (osc1.process() + osc2.process());
    float modLfoValue = modLfo.process();
    oscOut *= modLfoValue;
    volume = volumeLfo.process();
    oscOut *= 2.0f * volume;

    // implement gliss
    //note += noteVar;
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
        }
    }
    return oscOut;
}