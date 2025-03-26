/*
  ==============================================================================

    PitchShiftEffectProcessor.h
    Created: 4 Mar 2025 7:36:04pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#ifndef PitchShiftEffectProcessor_hpp
#define PitchShiftEffectProcessor_hpp

#include <cmath>
#include "PitchDelay.h"

using namespace std;


class PitchShiftEffectProcessor
{
public:

    PitchShiftEffectProcessor();
    
    ~PitchShiftEffectProcessor();
    
    
    float processSample(float x, int channel);

    void setFs(float Fs);
    
    void setPitch(float semitone);


private:
    float Fs = 48000.f;
    
    PitchDelay pitchDelay1{1};
    PitchDelay pitchDelay2{2};
    PitchDelay pitchDelay3{3};
    
    float a1[2] = {1.5f*M_PI,1.5f*M_PI};
    float a2[2] = {(1.5f*M_PI) + 2.f*M_PI/3.f ,
                    (1.5f*M_PI) + 2.f*M_PI/3.f};
    float a3[2] = {(1.5f*M_PI) + 4.f*M_PI/3.f ,
                     (1.5f*M_PI) + 4.f*M_PI/3.f};
    float angleChange;
    float freq;
    
    const float MAX_DELAY_SEC = .03f;
    float MAX_DELAY_SAMPLES = MAX_DELAY_SEC * Fs;
    
    float semitone = 0.f;
    float tr = 1.f; // momentary trasposition
    float delta = 0.f; // rate of change for delay (samples)
    
//    float pitchAmount = 0.0f; // Pitch Shift Amount
//    float pitchTarget = 0.0f; // Pitch Target for smooth and snap
//    float timeSnap = 0.0f; // [0-100] [0 = free, 100 = quanitze to beat]
//    float pitchCorrect = 0.0f; //[0-100] [0 = free, 100 = snap to scale notes]
//    int currentKey = 0; // 0=C, 1=C#, 2=D, etc.
//    
//    juce::dsp::FFT fft{11};
//    juce::AudioBuffer<float> fftBuffer;
//    int fftBufferIndex = 0;
//    float pitchShiftFactor = 1.0f; // [1 = no shift, 2 = octave up, 0.5 = octave down]
//    std::vector<juce::dsp::Complex<float>> fftOutput;
    
};





#endif /* PitchShifter_hpp */
