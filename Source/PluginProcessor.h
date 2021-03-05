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

    // function to generate random interval and implement it
    void updateRandomInterval()
    {
        int intrvlNum = (int)MQSSynth::IntrvlName::INTRVL_NR_ITEMS;
        interval = static_cast<MQSSynth::IntrvlName>(abs(random.nextInt()) % intrvlNum);
        for (int synthIndex = 0; synthIndex < synthNum; synthIndex++)
        {
            mqsSynths[synthIndex].setIntrvlbyType(interval);
        }
        begin = std::chrono::steady_clock::now();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MQSDroneAudioProcessor)
};
