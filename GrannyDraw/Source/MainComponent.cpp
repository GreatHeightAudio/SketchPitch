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
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.drawImage(m_pSharedImages->getBackgroundImage(), getLocalBounds().toFloat());
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    float scaleX = bounds.getWidth() / 728.0f;
    float scaleY = bounds.getHeight() / 600.0f;

    int knobW = static_cast<int>(120 * scaleX);
    int knobH = static_cast<int>(120 * scaleY);
    int knobX = static_cast<int>(-5.0f * scaleX);
    int knobY = static_cast<int>(500.0f * scaleY);

    bigKnob.setBounds(knobX, knobY, knobW, knobH);
}
