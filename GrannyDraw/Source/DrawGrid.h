/*
  ==============================================================================

    DrawGrid.h
    Created: 26 Mar 2025 3:22:15pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct CurvePoint
{
    float normalizedX; // 0.0 to 1.0 (left to right)
    float pitch;       // -12.0 to +12.0 (bottom to top)
};

class DrawGrid : public juce::Component
{
public:
    DrawGrid();

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    juce::Point<int> getClampedPoint(juce::Point<int> p) const;

    std::vector<CurvePoint> getPitchCurve() const;
    void setPitchCurve(const std::vector<CurvePoint>& newCurve);

    std::function<void()> onCurveFinished;

private:
    std::vector<CurvePoint> drawnCurvePoints;
};


