/*
  ==============================================================================

    DrawGrid.h
    Created: 26 Mar 2025 3:22:15pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DrawGrid : public juce::Component
{
public:
    DrawGrid();

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    juce::Point<int> getClampedPoint(juce::Point<int> p) const;

    std::vector<juce::Point<int>> getDrawnPoints() const;

    std::vector<float> getPitchCurve() const;

    std::function<void()> onCurveFinished;



private:
    std::vector<juce::Point<int>> drawnPoints;
    const int cellSize = 20;
    
};

