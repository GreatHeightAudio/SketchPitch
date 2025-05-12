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
: AudioProcessorEditor(&p), processor(p), pitchGrid(*p.getSharedImagesPtr()), mainComponent(p), modeComponent(p)
{
    setSize(refWidth, refHeight);
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(728.0 / 600.0);

    addAndMakeVisible(mainComponent);
    addAndMakeVisible(pitchGrid);
    addAndMakeVisible(modeComponent);
    addAndMakeVisible(shakeButton);
    shakeButton.onClick = [this]() {
        pitchGrid.reset();
        originalWindowPos = getTopLevelComponent()->getPosition();
        shakeStartTime = juce::Time::getMillisecondCounter();

        class ShakeTimer : public juce::Timer {
        public:
            ShakeTimer(GrannyDrawAudioProcessorEditor* ed) : editor(ed) {
                startTimerHz(60);
            }
            void timerCallback() override {
                auto now = juce::Time::getMillisecondCounter();
                if (now - editor->shakeStartTime > 200) {
                    stopTimer();
                    editor->getTopLevelComponent()->setTopLeftPosition(editor->originalWindowPos);
                    return;
                }
                int offsetX = juce::Random::getSystemRandom().nextInt(5) - 2;
                int offsetY = juce::Random::getSystemRandom().nextInt(5) - 2;
                editor->getTopLevelComponent()->setTopLeftPosition(
                    editor->originalWindowPos + juce::Point<int>(offsetX, offsetY)
                );
            }
        private:
            GrannyDrawAudioProcessorEditor* editor;
        };

        shakeTimer = std::make_unique<ShakeTimer>(this);
    };

    pitchGrid.onErased = [this]()
    {
        processor.setErasedRanges(pitchGrid.getErasedRanges());
    };
    
    if (! processor.getPitchCurve().empty())
        {
            pitchGrid.setPitchCurve(processor.getPitchCurve());
        }

    pitchGrid.onCurveFinished = [this]{
        sendPitchCurve();
    };
    
    modeComponent.onModeChanged = [this](DrawGrid::DrawMode newMode)
    {
        pitchGrid.setMode(newMode);
    };
    pitchGrid.onErased = [this]{
        processor.setErasedRanges(pitchGrid.getErasedRanges());
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
    
    int shakeX = (int)(600.0f * scaleX);
    int shakeY = (int)(20.0f * scaleY);
    int shakeW = (int)(100.0f * scaleX);
    int shakeH = (int)(30.0f * scaleY);

    shakeButton.setBounds(shakeX, shakeY, shakeW, shakeH);

    
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
