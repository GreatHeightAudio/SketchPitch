/*
  ==============================================================================

    SharedImages.h
    Created: 24 Apr 2025 3:45:05pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SharedImages {
    
public:
    SharedImages() { loadImages(); };
    
    void loadImages();
    
    juce::Image& getBackgroundImage() { return m_BackgroundImage; };
    
    juce::Image& getBigKnobImage()    { return m_BigKnob; };

    
    
private:
    juce::Image m_BackgroundImage;
    
    juce::Image m_BigKnob;

};

