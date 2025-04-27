/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DrawGrid.h"
#include "MainComponent.h"

//==============================================================================
/**
*/
class GrannyDrawAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Timer
{
public:
    GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor&);
    ~GrannyDrawAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
private:
    static constexpr int refWidth = 728;
    static constexpr int refHeight = 600;
    

    GrannyDrawAudioProcessor& processor;

    juce::Image sketchFrame;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> quantizeAttachment;

    juce::Rectangle<int> imageBounds;

    DrawGrid pitchGrid;
    void sendPitchCurve();

    MainComponent mainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrannyDrawAudioProcessorEditor)
};
