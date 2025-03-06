/*
  ==============================================================================

    PitchShiftEffectProcessor.cpp
    Created: 4 Mar 2025 7:36:04pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "PitchShiftEffectProcessor.h"

void PitchShiftEffectProcessor::processBuffer(juce::AudioBuffer<float> & buffer, const int channel, const int numSamples)
{
    for (int n = 0 ; n < numSamples ; ++n){
        float x = buffer.getSample(channel, n); //current sample
        fftBuffer.setSample(0, fftBufferIndex, x);
        fftBufferIndex++;
        
        if (fftBufferIndex >= fft.getSize()){
            fftBufferIndex = 0;
        }
        juce::dsp::Complex<float>* fftData = reinterpret_cast<juce::dsp::Complex<float>*>(fftBuffer.getWritePointer(0));
        fft.perform(fftData, false);
        
    }
}


void PitchShiftEffectProcessor::prepare(float sampleRate)
{
    Fs = sampleRate;
}

void PitchShiftEffectProcessor::setPitchAmount(float newAmount)
{
    pitchAmount = newAmount;
}

