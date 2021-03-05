/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MQSDroneAudioProcessorEditor::MQSDroneAudioProcessorEditor (MQSDroneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    refreshTimer = new RefreshTimer(200, [this]() { this->updateIntrvlInfo(); });
    signature.setItalic(true);
    signature.setHeight(16.0f);
}

MQSDroneAudioProcessorEditor::~MQSDroneAudioProcessorEditor()
{
    refreshTimer->stopTimer();
}

//==============================================================================
void MQSDroneAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(bgColour);
    g.setColour(Colours::white);

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawFittedText (displayText, getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(signature);
    g.drawFittedText("MQS MIDI CHORDS ANALYZER", getLocalBounds(), Justification::bottomRight, 1);
}

void MQSDroneAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
