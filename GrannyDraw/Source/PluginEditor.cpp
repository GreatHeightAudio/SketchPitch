/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    float scalar = 1.f;
    int windowWidth = (int) (scalar * refWidth);
    int windowHeight = (int) (scalar * refHeight);
    setSize (windowWidth, windowHeight);
    
    int xPos = (int) ((50.f / refWidth) * windowWidth);
    int yPos = (int) ((70.f / refHeight) * windowHeight);
    int width = (int) ((100.f / refWidth) * windowWidth);
    int height = (int) ((100.f / refHeight) * windowHeight);
    
//    pitchKnob.addListener(this);
//    pitchKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
//    pitchKnob.setBounds(xPos,yPos, width, height);
//    pitchKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 30);
//    pitchKnob.setRange(-12.0f,12.f,0.1f);
//    pitchKnob.setValue(0.f);
//    addAndMakeVisible(pitchKnob);
//    
//    sliderAttachments = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
//            processor.state,"PITCH",pitchKnob);
    
    addAndMakeVisible(pitchGrid);

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
    pitchGrid.setBounds(20, 20, getWidth() - 40, getHeight() - 120);

}

//void GrannyDrawAudioProcessorEditor::sliderValueChanged(Slider * slider)
//{
//    if (slider == &pitchKnob){
//        processor.pitchValue = pitchKnob.getValue();
//    }
//    
//}
void GrannyDrawAudioProcessorEditor::sendPitchCurve()
{
    auto newCurve = pitchGrid.getPitchCurve();
    processor.setPitchCurve(newCurve);
}
