/*
  ==============================================================================

    CursorBall.h
    Created: 20 Apr 2025 2:33:44pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class CursorBall  : public juce::Component
{
public:
    CursorBall();
    ~CursorBall() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CursorBall)
};
