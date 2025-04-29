/*
  ==============================================================================

    MainComponent.cpp
    Created: 24 Apr 2025 3:44:55pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "MainComponent.h"

MainComponent::MainComponent(GrannyDrawAudioProcessor& p)
    : audioProcessor(p),
      m_pSharedImages(p.getSharedImagesPtr()),
      snapKnob(m_pSharedImages, "SNAP"),
      loopRateKnob(m_pSharedImages, "RATE")
{
    snapKnob.setRange(0.0, 100.0);

    snapKnobAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters,
                                                                                                 "snap", snapKnob);
        
    addAndMakeVisible(snapKnob);
    addAndMakeVisible(loopRateKnob);
    
    loopRateAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters,
                                                                                                 "loopRate", loopRateKnob);

    addAndMakeVisible(loopRateKnob);
    
    loopRateLabel.setText("1 Bar", juce::dontSendNotification);
    loopRateLabel.setJustificationType(juce::Justification::centred);
    loopRateLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    loopRateLabel.setFont(juce::Font(16.0f));
    loopRateKnob.onValueChange = [this]()
    {
        static const juce::String labels[] = {
            "4 Bars", "2 Bars", "1 Bar", "1/2", "1/4", "1/8", "1/16"
        };

        int index = (int) loopRateKnob.getValue();
        index = juce::jlimit(0, 6, index);

        loopRateLabel.setText(labels[index], juce::dontSendNotification);
    };
    loopRateKnob.onValueChange(); // manually trigger it once - bug

    
    addAndMakeVisible(loopRateLabel);
    
    snapLabel.setText("0%", juce::dontSendNotification);
    snapLabel.setJustificationType(juce::Justification::centred);
    snapLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    snapLabel.setFont(juce::Font(16.0f));
    addAndMakeVisible(snapLabel);

    snapKnob.onValueChange = [this]()
    {
        int snapValue = (int) snapKnob.getValue();
        snapLabel.setText(juce::String(snapValue) + "%", juce::dontSendNotification);
    };
}

MainComponent::~MainComponent() {}

void MainComponent::paint(juce::Graphics& g)
{
    if (m_pSharedImages != nullptr)
        {
            auto& bg = m_pSharedImages->getBackgroundImage();

            if (bg.isValid())
                g.drawImage(bg, getLocalBounds().toFloat());
        }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    constexpr float refWidth = 728.0f;
    constexpr float refHeight = 600.0f;

    float scaleX = (float)bounds.getWidth() / refWidth;
    float scaleY = (float)bounds.getHeight() / refHeight;

    int knobW = (int)(120.0f * scaleX);
    int knobH = (int)(120.0f * scaleY);
    int knobY = (int)(500.0f * scaleY);

    snapKnob.setBounds((int)(-5.0f * scaleX), knobY, knobW, knobH);
    loopRateKnob.setBounds(bounds.getWidth() - knobW + (int)(5.0f * scaleX), knobY, knobW, knobH);
    
    auto loopRateBounds = loopRateKnob.getBounds();
    loopRateLabel.setBounds(
        loopRateBounds.getX() + knobW / 2 - 35,
        loopRateBounds.getY() - 30,
        100,
        20
    );
    auto snapBounds = snapKnob.getBounds();
    snapLabel.setBounds(
        snapBounds.getX() + knobW / 2 - 65,
        snapBounds.getY() - 30,
        100,
        20
    );

}
