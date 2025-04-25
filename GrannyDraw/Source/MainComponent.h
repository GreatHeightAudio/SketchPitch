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


//==============================================================================
/*
*/
class MainComponent  : public juce::Component
{
public:
    MainComponent(GrannyDrawAudioProcessor&);
    ~MainComponent() override;

    void updateImageBounds(juce::Rectangle<int> newBounds);
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseEnter (const juce::MouseEvent &event) override;

private:
    GrannyDrawAudioProcessor&   audioProcessor;
    SharedImages*               m_pSharedImages;
    BigKnob                     bigKnob;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> quantizeAttachment;
    
    juce::Rectangle<int> imageBounds;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    
};

