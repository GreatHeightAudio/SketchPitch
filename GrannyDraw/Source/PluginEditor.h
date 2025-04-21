/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DrawGrid.h"

//==============================================================================
/**
*/
class GrannyDrawAudioProcessorEditor  : public juce::AudioProcessorEditor,
//                                        public juce::Slider::Listener
                                        public juce::Timer
{
public:
    GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor&);
    ~GrannyDrawAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
//    void sliderValueChanged(juce::Slider *slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    float refWidth = 450.f;
    float refHeight = 300.f;
    
    GrannyDrawAudioProcessor& processor;
    
    juce::Image sketchFrame;
//    juce::Label quantizeLabel;
    ImageStripKnob quantizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> quantizeAttachment;

    juce::Rectangle<int> imageBounds;


    DrawGrid pitchGrid;
    void sendPitchCurve();
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrannyDrawAudioProcessorEditor)
};
