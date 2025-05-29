/*
  ==============================================================================

    ModeComponent.h
    Created: 30 Apr 2025 9:03:58pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SharedImages.h"

class ModeComponent : public juce::Component,
                      public juce::ComboBox::Listener
{
public:
    ModeComponent(GrannyDrawAudioProcessor&, SharedImages& images);
    ~ModeComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    std::function<void(DrawGrid::DrawMode)> onModeChanged;

private:
    juce::ComboBox styleBox;
    SharedImages& sharedImages;
    class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                              int buttonX, int buttonY, int buttonW, int buttonH,
                              juce::ComboBox& box) override
            {
                if (comboImage.isValid())
                    g.drawImageWithin(comboImage, 0, 0, width, height,
                                      juce::RectanglePlacement::stretchToFit);
                else
                    g.fillAll(juce::Colours::grey);
            }
        
        void drawLabel(juce::Graphics& g, juce::Label& label) override
            {
                g.fillAll(juce::Colours::transparentBlack);
            }

        void setComboImage(const juce::Image& img) { comboImage = img; }

    private:
            juce::Image comboImage;
        };

        CustomLookAndFeel customLF;
    };
