/*
  ==============================================================================

    PitchShiftEffectProcessor.h
    Created: 4 Mar 2025 7:36:04pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PitchShiftEffectProcessor
{
public:

    void processBuffer(juce::AudioBuffer<float> & buffer, const int channel, const int numSamples);
    void prepare(float sampleRate);
    void setPitchAmount(float newAmount);

private:
    float pitchAmount = 0.0f; // Pitch Shift Amount
    float pitchTarget = 0.0f; // Pitch Target for smooth and snap
    float timeSnap = 0.0f; // [0-100] [0 = free, 100 = quanitze to beat]
    float pitchCorrect = 0.0f; //[0-100] [0 = free, 100 = snap to scale notes]
    int currentKey = 0; // 0=C, 1=C#, 2=D, etc.
    float Fs = 48000.f;
    juce::dsp::FFT fft{11};
    juce::AudioBuffer<float> fftBuffer;
    int fftBufferIndex = 0;
    float pitchShiftFactor = 1.0f; // [1 = no shift, 2 = octave up, 0.5 = octave down]
    std::vector<juce::dsp::Complex<float>> fftOutput;
    
};
