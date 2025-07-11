/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DrawGrid.h"

using namespace juce;

GrannyDrawAudioProcessorEditor::GrannyDrawAudioProcessorEditor(GrannyDrawAudioProcessor& p)
: AudioProcessorEditor(&p), processor(p), pitchGrid(*p.getSharedImagesPtr()), mainComponent(p), modeComponent(p, *p.getSharedImagesPtr())
{
    setSize(refWidth, refHeight);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(728.0 / 600.0);
    

    addAndMakeVisible(mainComponent);
    addAndMakeVisible(pitchGrid);
    addAndMakeVisible(modeComponent);
    shakeButton.setImages(false, true, true,
                          processor.getSharedImagesPtr()->shakeButtonUp(),
                          1.0f, {},
                          processor.getSharedImagesPtr()->shakeButtonUp(),
                          1.0f, {},
                          processor.getSharedImagesPtr()->shakeButtonDown(),
                          1.0f, {});

    addAndMakeVisible(shakeButton);

    shakeButton.onClick = [this]() {
        pitchGrid.reset();
        originalWindowPos = getTopLevelComponent()->getPosition();
        shakeStartTime = juce::Time::getMillisecondCounter();
        processor.setErasedRanges({});
        processor.pitchCurve.clear();

        shakeTimer = std::make_unique<ShakeTimer>(this);
    };


    pitchGrid.onErased = [this]()
    {
        const auto& ranges = pitchGrid.getErasedRegions();
        processor.setErasedRanges(ranges);

        auto fullCurve = pitchGrid.getFullPitchCurve();
        processor.setPitchCurve(fullCurve);
    };

    
    if (! processor.getPitchCurve().empty())
        {
            pitchGrid.setPitchCurve(processor.getPitchCurve());
            pitchGrid.setPitchCurveReference(processor.getPitchCurvePointer());
            pitchGrid.setResampledCurve(processor.getResampledPitchCurve());
            processor.setErasedRanges(pitchGrid.getErasedRegions());
        }

    pitchGrid.onCurveFinished = [this]{
        sendPitchCurve();
    };
    
    modeComponent.onModeChanged = [this](DrawGrid::DrawMode newMode)
    {
        pitchGrid.setMode(newMode);
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
    
    int modeX = (int)(45.0f * scaleX);
    int modeY = (int)(20.0f * scaleY);
    int modeW = (int)(110.0f * scaleX);
    int modeH = (int)(40.0f * scaleY);

    modeComponent.setBounds(modeX, modeY, modeW, modeH);

    mainComponent.setBounds(bounds);
    
    int shakeX = (int)(575.0f * scaleX);
    int shakeY = (int)(20.0f * scaleY);
    int shakeW = (int)(110.0f * scaleX);
    int shakeH = (int)(40.0f * scaleY);

    shakeButton.setBounds(shakeX, shakeY, shakeW, shakeH);

    
}

void GrannyDrawAudioProcessorEditor::timerCallback()
{
    const auto& curve = processor.getPitchCurve();
    size_t curveLength = curve.size();
    if (curveLength == 0)
        return;

    pitchGrid.setPlayheadPhase(processor.getPlayheadPhase());
    
    if (processor.needsCurveUpdate.exchange(false))
        {
            pitchGrid.setPitchCurve(processor.getPitchCurve());
        }
}




void GrannyDrawAudioProcessorEditor::sendPitchCurve()
{
    auto fullCurve = pitchGrid.getFullPitchCurve();
    processor.setPitchCurve(fullCurve);
    processor.setErasedRanges(pitchGrid.getErasedRegions());
    pitchGrid.setPitchCurveReference(&processor.getResampledPitchCurve());
    auto curveLength = processor.getPitchCurveLength();
    startTimerHz(static_cast<int>(curveLength / 2));
}

