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
    BigKnob(SharedImages* i, const juce::String& labelTextToUse)
        : knobImage(i->getBigKnobImage()), labelText(labelTextToUse)
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    }

    ~BigKnob() {}

    void paint(juce::Graphics& g) override
    {
        const double fractRotation = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        
        int frameIndex = juce::jlimit(0, numFrames - 1, (int)std::floor(fractRotation * (numFrames - 1)));
        int sourceY = frameIndex * frameHeight;

        g.drawImage(knobImage, 0, 0, getWidth(), getHeight(), 0, sourceY, frameWidth, frameHeight);

        g.setColour(juce::Colours::darkgrey);
        juce::Font labelFont(15.0f);
        g.setFont(labelFont);

        auto textBounds = getLocalBounds().translated(0, 13);
        g.drawFittedText(labelText, textBounds, juce::Justification::centred, 1);
    }

private:
    juce::Image& knobImage;
    juce::String labelText;

    int numFrames = 245;
    int frameHeight = 490;
    int frameWidth = 430;
};
