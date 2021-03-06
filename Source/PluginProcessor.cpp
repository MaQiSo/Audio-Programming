/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MQSDroneAudioProcessor::MQSDroneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (int synthIndex = 0; synthIndex < synthNum; synthIndex++) 
    {
        mqsSynths.push_back(MQSSynth());
    }
    for (int fallIndex = 0; fallIndex < fallNum; fallIndex++)
    {
        mqsInFalls.push_back(MQSInFall());
    }
}

MQSDroneAudioProcessor::~MQSDroneAudioProcessor()
{
}

//==============================================================================
const juce::String MQSDroneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MQSDroneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MQSDroneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MQSDroneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MQSDroneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MQSDroneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MQSDroneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MQSDroneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MQSDroneAudioProcessor::getProgramName (int index)
{
    return {};
}

void MQSDroneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MQSDroneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    /*sinOsc.setSampleRate(sampleRate);
    sinOsc.setFrequency(440.0f);*/

    updateRandomInterval();

    for (int synthIndex = 0; synthIndex < synthNum; synthIndex++)
    {
        mqsSynths[synthIndex].setSampleRate(sampleRate);
    }
    for (int fallIndex = 0; fallIndex < fallNum; fallIndex++)
    {
        mqsInFalls[fallIndex].setSampleRate(sampleRate);
    }
}

void MQSDroneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MQSDroneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MQSDroneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    // DSP Loop:
    for (int i = 0; i < numSamples; i++)
    {
        float left{ 0.0f };
        float right{ 0.0f };
        int checkThrough{ 0 };

        // generate MQS synths
        for (int synthIndex = 0; synthIndex < synthNum; synthIndex++)
        {
            left += mqsSynths[synthIndex].processL();
            right += mqsSynths[synthIndex].processR();
            checkThrough += (int)mqsSynths[synthIndex].isThrough();
        }

        end = std::chrono::steady_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
        if (checkThrough == synthNum && duration > 8)
        {
            updateRandomInterval();
            DBG("changed");
        }

        // generate MQS in falls
        for (int fallIndex = 0; fallIndex < fallNum; fallIndex++)
        {
            left += mqsInFalls[fallIndex].processL();
            right += mqsInFalls[fallIndex].processR();
        }

        left /= (float)synthNum + (float)fallNum;
        right /= (float)synthNum + (float)fallNum;

        // output limiter
        left = tanhf(left);
        right = tanh(right);

        leftChannel[i] = left;
        rightChannel[i] = right;
    }
}

void MQSDroneAudioProcessor::updateRandomInterval()
{
    int intrvlNum = (int)MQSSynth::IntrvlName::INTRVL_NR_ITEMS;
    interval = static_cast<MQSSynth::IntrvlName>(abs(random.nextInt()) % intrvlNum);
    float seed = (float)random.nextInt();
    for (int synthIndex = 0; synthIndex < synthNum; synthIndex++)
    {
        mqsSynths[synthIndex].setIntrvlbyType(interval);
        mqsSynths[synthIndex].setSeedNote(seed);
    }
    begin = std::chrono::steady_clock::now();
}

//==============================================================================
bool MQSDroneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MQSDroneAudioProcessor::createEditor()
{
    return new MQSDroneAudioProcessorEditor (*this);
}

//==============================================================================
void MQSDroneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MQSDroneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MQSDroneAudioProcessor();
}
