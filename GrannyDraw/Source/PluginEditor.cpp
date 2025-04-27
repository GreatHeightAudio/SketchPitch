/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


using namespace juce;

//==============================================================================
GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor (GrannyDrawAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), mainComponent(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(refWidth, refHeight);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(728.0 / 600.0);
    
//    sketchFrame = juce::ImageCache::getFromMemory(BinaryData::Sketch__Pitch_png, BinaryData::Sketch__Pitch_pngSize);
    
    addAndMakeVisible(mainComponent);
    addAndMakeVisible(pitchGrid);
    mainComponent.getBounds();
    resized();
    
    pitchGrid.onCurveFinished = [this]{
        sendPitchCurve();
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
    
    mainComponent.setBounds(getLocalBounds());
    mainComponent.updateImageBounds(imageBounds);


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

