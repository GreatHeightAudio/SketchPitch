/*
  ==============================================================================

    ModeComponent.h
    Created: 30 Apr 2025 9:03:58pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ModeComponent : public juce::Component,
                      public juce::ComboBox::Listener
{
public:
    ModeComponent(GrannyDrawAudioProcessor&);
    ~ModeComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    juce::ComboBox styleBox;
};
