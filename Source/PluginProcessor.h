/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "Oscillators.h"
#include "MQSInFall.h"
#include "MQSSynth.h"
#include <chrono>

//==============================================================================
/**
*/
class MQSDroneAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MQSDroneAudioProcessor();
    ~MQSDroneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    MQSSynth::IntrvlName interval{ MQSSynth::IntrvlName::major2 };

private:
    //==============================================================================
    // SinOsc sinOsc;
    std::vector<MQSSynth> mqsSynths;
    std::vector<MQSInFall> mqsInFalls;
    int synthNum{ 6 };
    int fallNum{ 6 };
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    Random random;

    // function to generate random global interval and implement it
    void updateRandomInterval();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MQSDroneAudioProcessor)
};
