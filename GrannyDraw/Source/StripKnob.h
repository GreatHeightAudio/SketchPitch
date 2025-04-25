/*
  ==============================================================================

    StripKnob.h
    Created: 20 Apr 2025 9:43:28pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SharedImages.h"

class BigKnob : public juce::Slider
{
public:
    BigKnob(SharedImages* i) : knobImage(i->getBigKnobImage())
    {
        
        setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        
    }
    
    
    ~BigKnob() {};
    
    void paint(juce::Graphics& g) override
    {
        /* Normalizing the slider value */
        const double fractRotation = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        
        int frameIndex = floor (fractRotation * (numFrames - 1));
        
        const int startY = frameIndex * frameHeight;
        
        g.drawImage(knobImage, 0, 0, getWidth(), getHeight(), 0, startY, frameWidth, frameHeight);
        
    }

    
    
    
private:
    juce::Image& knobImage;
    
    int numFrames = 245;
    
    int frameHeight = 490;
    
    int frameWidth = 430;
    
    
};
