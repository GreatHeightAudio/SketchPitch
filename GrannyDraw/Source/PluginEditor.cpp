/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    pitchKnob.addListener(this);
    pitchKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    pitchKnob.setBounds(50,74,100,100);
    pitchKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
    pitchKnob.setRange(-12.0f,12.f,0.1f);
    pitchKnob.setValue(0.f);
    addAndMakeVisible(pitchKnob);
    
    sliderAttachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            processor.state,"PITCH",pitchKnob);
}

GrannyDrawAudioProcessorEditor::~GrannyDrawAudioProcessorEditor()
{
}

//==============================================================================
void GrannyDrawAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void GrannyDrawAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
