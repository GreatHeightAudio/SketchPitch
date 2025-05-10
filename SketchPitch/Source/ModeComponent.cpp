/*
  ==============================================================================

    ModeComponent.cpp
    Created: 30 Apr 2025 9:03:58pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "ModeComponent.h"

ModeComponent::ModeComponent(GrannyDrawAudioProcessor&)
{
    addAndMakeVisible(styleBox);
    styleBox.addItem("Solo", 1);
    styleBox.addItem("Layer", 2);
    styleBox.addItem("Erase", 3);
    styleBox.setSelectedId(1);
    styleBox.addListener(this);
}

ModeComponent::~ModeComponent() {}

void ModeComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void ModeComponent::resized()
{
    styleBox.setBounds(getLocalBounds());
}


void ModeComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &styleBox)
    {
        int selectedId = styleBox.getSelectedId();
        
        if (onModeChanged != nullptr)
        {
            switch (selectedId)
            {
                case 1: onModeChanged(DrawGrid::DrawMode::Solo); break;
                case 2: onModeChanged(DrawGrid::DrawMode::Layer); break;
                case 3: onModeChanged(DrawGrid::DrawMode::Erase); break;
            }
        }
    }
}
