/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "KnobStrip.h"

using namespace juce;

//==============================================================================
GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    float scalar = 1.f;
    int windowWidth = (int) (scalar * refWidth);
    int windowHeight = (int) (scalar * refHeight);
    setSize (windowWidth, windowHeight);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(728.0 / 600.0);
    
    sketchFrame = juce::ImageCache::getFromMemory(BinaryData::Sketch__Pitch_png, BinaryData::Sketch__Pitch_pngSize);

    
    quantizeSlider.setSliderStyle(juce::Slider::Rotary);
    quantizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(quantizeSlider);
    quantizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                                processor.parameters, "snap", quantizeSlider);

//    quantizeLabel.setText("snap", juce::dontSendNotification);
//    quantizeLabel.attachToComponent(&quantizeSlider, false);
//    addAndMakeVisible(quantizeLabel);

    juce::OwnedArray<juce::Image> knobFrames;

    for (int i = 0; i < 128; ++i)
    {
        juce::String name = "knob_" + juce::String(i).paddedLeft('0', 3) + "_png";
        auto* img = juce::ImageFileFormat::loadFrom(BinaryData::getNamedResource(name.toRawUTF8(), size));
        knobFrames.add(new juce::Image(*img));
    }

    quantizeSlider.setImageFrames(knobFrames);

    
    addAndMakeVisible(pitchGrid);
    pitchGrid.onCurveFinished = [this]{
        sendPitchCurve();
        resized();
    };

Timer::startTimerHz (60);
}

GrannyDrawAudioProcessorEditor::~GrannyDrawAudioProcessorEditor()
{
    Timer::stopTimer();
}

//==============================================================================
void GrannyDrawAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (sketchFrame.isValid())
    {
        g.drawImage(sketchFrame, imageBounds.toFloat());
    }
}


void GrannyDrawAudioProcessorEditor::resized()
{
    // Step 1: Calculate image size
    imageBounds = getLocalBounds().withSizeKeepingCentre(
        (int)(getHeight() * (728.0 / 600.0)),
        getHeight()
    );

    auto scaleX = (float)imageBounds.getWidth() / 728.0f;
    auto scaleY = (float)imageBounds.getHeight() / 600.0f;

    // Step 2: Set grid inside screen cutout
    int screenX = (int)(80 * scaleX);
    int screenY = (int)(75 * scaleY);
    int screenW = (int)(570 * scaleX);
    int screenH = (int)(425 * scaleY);

    pitchGrid.setBounds(imageBounds.getX() + screenX, imageBounds.getY() + screenY, screenW, screenH);
    
    int knobX = imageBounds.getX() + (int)(-30 * scaleX);
    int knobY = imageBounds.getBottom() - (int)(115 * scaleY);
    int knobW = (int)(160 * scaleX);
    int knobH = (int)(160 * scaleY);
    quantizeSlider.setBounds(knobX, knobY, knobW, knobH);

    
    repaint();
}




void GrannyDrawAudioProcessorEditor::timerCallback()
{
        size_t curveLength = processor.getPitchCurveLength();
        if (curveLength == 0)
            return;

        int index = processor.getPitchPlayheadIndex();
        index = (index + 1) % curveLength; // loop back at end

        processor.setPitchPlayheadIndex(index);

}

void GrannyDrawAudioProcessorEditor::sendPitchCurve()
{
    auto newCurve = pitchGrid.getPitchCurve();
    processor.setPitchCurve(newCurve);
    
    auto curveLength = processor.getPitchCurveLength();
    startTimerHz(static_cast<int>(curveLength / 2));

}

