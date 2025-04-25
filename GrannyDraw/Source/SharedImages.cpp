/*
  ==============================================================================

    SharedImages.cpp
    Created: 24 Apr 2025 3:45:05pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "SharedImages.h"

void SharedImages::loadImages()
{
    
    m_BackgroundImage = juce::ImageCache::getFromMemory(BinaryData::Sketch__Pitch_png, BinaryData::Sketch__Pitch_pngSize);
    
    m_BigKnob         = juce::ImageCache::getFromMemory(BinaryData::Knob_big_png, BinaryData::Knob_big_pngSize);

    
}

