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
    
    m_shakeButtonUp       = juce::ImageCache::getFromMemory(BinaryData::ShakeButtonLight_png, BinaryData::ShakeButtonLight_pngSize);
    
    m_shakeButtonDown       = juce::ImageCache::getFromMemory(BinaryData::ShakeButtonDark_png, BinaryData::ShakeButtonDark_pngSize);
    
    m_modeButtonUp          = juce::ImageCache::getFromMemory(BinaryData::ModeButtonLight_png, BinaryData::ModeButtonLight_pngSize);
    
    m_modeButtonDown        = juce::ImageCache::getFromMemory(BinaryData::ModeButtonDark_png, BinaryData::ModeButtonDark_pngSize);
}

