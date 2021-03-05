/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RefreshTimer : public Timer
{
public:
    RefreshTimer(int ms, std::function<void()> fn) :func(fn)
    {
        startTimer(ms);
    }
    ~RefreshTimer()
    {
        stopTimer();
    }
    void timerCallback() override
    {
        func();
    }
private:
    std::function<void()> func;
};

class MQSDroneAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MQSDroneAudioProcessorEditor (MQSDroneAudioProcessor&);
    ~MQSDroneAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    String displayText;

    MQSDroneAudioProcessor& audioProcessor;

    Colour bgColour{ Colour(0xff6DABE4) };
    Font signature;

    void updateIntrvlInfo()
    {
        String intrvlName = getIntrvlName(audioProcessor.interval);
        displayText = "Hello, " + intrvlName + " World!";
        repaint();
    }

    RefreshTimer* refreshTimer;

    String getIntrvlName(MQSSynth::IntrvlName _intrvl)
    {
        switch (_intrvl)
        {
        case MQSSynth::IntrvlName::minor2:
            return "Minor second";
        case MQSSynth::IntrvlName::major2:
            return "Major second";
        case MQSSynth::IntrvlName::minor3:
            return "Minor third";
        case MQSSynth::IntrvlName::major3:
            return "Major third";
        case MQSSynth::IntrvlName::perfect4:
            return "Perfect fourth";
        case MQSSynth::IntrvlName::perfect5:
            return "Perfect fifth";
        case MQSSynth::IntrvlName::minor6:
            return "Minor sixth";
        case MQSSynth::IntrvlName::major6:
            return "Major sixth";
        }
        return " ";
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MQSDroneAudioProcessorEditor)
};
