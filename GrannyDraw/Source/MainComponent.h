/*
  ==============================================================================

    MainComponent.h
    Created: 24 Apr 2025 3:44:55pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SharedImages.h"
#include "StripKnob.h"

class MainComponent : public juce::Component
{
public:
    MainComponent(GrannyDrawAudioProcessor&);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    GrannyDrawAudioProcessor&   audioProcessor;
    SharedImages*               m_pSharedImages;
    BigKnob                     snapKnob;
    BigKnob                     loopRateKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loopRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snapKnobAttachment;
    juce::Label                 loopRateLabel;
    juce::Label                 snapLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
