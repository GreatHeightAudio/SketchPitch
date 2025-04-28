/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchShiftEffectProcessor.h"
#include "SharedImages.h"
#include "DrawGrid.h"

//==============================================================================
/**
*/
class GrannyDrawAudioProcessor  : public juce::AudioProcessor

{
public:
    //==============================================================================
    GrannyDrawAudioProcessor();
    ~GrannyDrawAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void setPitchCurve(const std::vector<CurvePoint>& newCurve);
        std::vector<CurvePoint> getPitchCurve() const;
    
    void setPitchPlayheadIndex(int index);
    int getPitchPlayheadIndex() const;
    size_t getPitchCurveLength() const;
    std::atomic<bool> needsCurveUpdate { false };
    
    SharedImages* getSharedImagesPtr() { return m_pSharedImagesPtr; };

    
private:
    PitchShiftEffectProcessor pitchShiftEffect;
    
    juce::SharedResourcePointer<SharedImages>     m_pSharedImagesPtr;

    juce::SmoothedValue<float> smoothedPitch;
    int pitchPlayhead;
    double pitchCurveDuration = 2; //(seconds)
    static double sampleCounter;
    
    std::vector<CurvePoint> pitchCurve;
  
    
    juce::AudioPlayHead *playHead;
    juce::AudioPlayHead::CurrentPositionInfo cpi;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrannyDrawAudioProcessor)
};
