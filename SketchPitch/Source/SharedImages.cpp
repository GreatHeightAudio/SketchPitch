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
    
    m_soloCursor      = juce::ImageCache::getFromMemory(BinaryData::solopencil_png, BinaryData::solopencil_pngSize);

    m_multiCursor      = juce::ImageCache::getFromMemory(BinaryData::multipencil_png, BinaryData::multipencil_pngSize);
    
    m_eraserCursor      = juce::ImageCache::getFromMemory(BinaryData::erase_png, BinaryData::erase_pngSize);
}

