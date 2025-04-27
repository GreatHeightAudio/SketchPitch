/*
  ==============================================================================

    MainComponent.cpp
    Created: 24 Apr 2025 3:44:55pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(GrannyDrawAudioProcessor& p) :
    audioProcessor          (p),
    m_pSharedImages         (audioProcessor.getSharedImagesPtr()),
    bigKnob                 (m_pSharedImages)
{
    quantizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "snap", bigKnob);
    addAndMakeVisible(bigKnob);
    
    updateImageBounds(getLocalBounds());
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.drawImage(m_pSharedImages->getBackgroundImage(), getLocalBounds().toFloat());
}

void MainComponent::updateImageBounds(juce::Rectangle<int> newBounds)
{
    imageBounds = newBounds;
    resized();
}


void MainComponent::resized()
{
    float scaleX = imageBounds.getWidth() / 728.0f;
    float scaleY = imageBounds.getHeight() / 600.0f;

    int knobW = static_cast<int>(120 * scaleX);
    int knobH = static_cast<int>(120 * scaleY);
    int knobX = imageBounds.getX() + static_cast<int>(-5.0 * scaleX);
    int knobY = imageBounds.getY() + static_cast<int>(500 * scaleY);

    bigKnob.setBounds(knobX, knobY, knobW, knobH);
    bigKnob.repaint();
}


void MainComponent::mouseEnter(const juce::MouseEvent &event)
{
    
}
