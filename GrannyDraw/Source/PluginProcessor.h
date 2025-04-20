/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchShiftEffectProcessor.h"

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

//    float pitchValue = 0.0f;
    
//    juce::AudioProcessorValueTreeState state;
//    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void setPitchCurve(const std::vector<float>& newCurve);
    std::vector<float> getPointCruve() const;
    
    void setPitchPlayheadIndex(int index);
    int getPitchPlayheadIndex() const;
    size_t getPitchCurveLength() const;


    
private:
    PitchShiftEffectProcessor pitchShiftEffect;
    
    std::vector<float> pitchCurve;
    std::vector<float> getPitchCurve() const;
    
    int pitchPlayhead;
    double pitchCurveDuration = 2; //(seconds)
    static double sampleCounter;
    
    juce::AudioPlayHead *playHead;
    juce::AudioPlayHead::CurrentPositionInfo cpi;
//    juce::AudioPlayHead::PositionInfo::getTimeSignature();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrannyDrawAudioProcessor)
};
