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
    
    juce::Image& getSoloCursor()    { return m_soloCursor; };
    
    juce::Image& getMultiCursor()    { return m_multiCursor; };

    juce::Image& getEraserCursor()    { return m_eraserCursor; };
    
private:
    juce::Image m_BackgroundImage;
    
    juce::Image m_BigKnob;
    
    juce::Image m_soloCursor;
    
    juce::Image m_multiCursor;
    
    juce::Image m_eraserCursor;

};

