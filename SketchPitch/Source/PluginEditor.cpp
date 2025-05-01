/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor(GrannyDrawAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), mainComponent(p), modeComponent(p)
{
    setSize(refWidth, refHeight);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(728.0 / 600.0);

    addAndMakeVisible(mainComponent);
    addAndMakeVisible(pitchGrid);
    addAndMakeVisible(modeComponent);
    
    if (! processor.getPitchCurve().empty())
        {
            pitchGrid.setPitchCurve(processor.getPitchCurve());
        }

    pitchGrid.onCurveFinished = [this]{
        sendPitchCurve();
    };

    Timer::startTimerHz(60);
}

GrannyDrawAudioProcessorEditor::~GrannyDrawAudioProcessorEditor()
{
    Timer::stopTimer();
}

void GrannyDrawAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}

void GrannyDrawAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    float scaleX = (float)bounds.getWidth() / 728.0f;
    float scaleY = (float)bounds.getHeight() / 600.0f;

    int screenX = (int)(80.0f * scaleX);
    int screenY = (int)(75.0f * scaleY);
    int screenW = (int)(570.0f * scaleX);
    int screenH = (int)(425.0f * scaleY);

    pitchGrid.setBounds(screenX, screenY, screenW, screenH);
    
    int modeX = (int)(20.0f * scaleX);
    int modeY = (int)(20.0f * scaleY);
    int modeW = (int)(160.0f * scaleX);
    int modeH = (int)(30.0f * scaleY);

    modeComponent.setBounds(modeX, modeY, modeW, modeH);

    mainComponent.setBounds(bounds);
    
}

void GrannyDrawAudioProcessorEditor::timerCallback()
{
    size_t curveLength = processor.getPitchCurveLength();
    if (curveLength == 0)
        return;

    int index = processor.getPitchPlayheadIndex();
    index = (index + 1) % curveLength;

    processor.setPitchPlayheadIndex(index);
    
    if (processor.needsCurveUpdate.exchange(false))
        {
            pitchGrid.setPitchCurve(processor.getPitchCurve());
        }
}

void GrannyDrawAudioProcessorEditor::sendPitchCurve()
{
    auto newCurve = pitchGrid.getPitchCurve();
    processor.setPitchCurve(newCurve);

    auto curveLength = processor.getPitchCurveLength();
    startTimerHz(static_cast<int>(curveLength / 2));
}
