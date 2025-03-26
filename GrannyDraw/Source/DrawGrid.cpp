/*
  ==============================================================================

    DrawGrid.cpp
    Created: 26 Mar 2025 3:22:15pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "DrawGrid.h"

DrawGrid::DrawGrid()
{
    setOpaque(true);
}

void DrawGrid::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::blue);

    g.setColour(juce::Colours::darkgrey);
    for (int x = 0; x < getWidth(); x += cellSize)
        g.drawLine((float)x, 0.f, (float)x, (float)getHeight());

    for (int y = 0; y < getHeight(); y += cellSize)
        g.drawLine(0.f, (float)y, (float)getWidth(), (float)y);

    g.setColour(juce::Colours::cyan);
    for (size_t i = 1; i < drawnPoints.size(); ++i)
    {
        auto p1 = drawnPoints[i - 1].toFloat();
        auto p2 = drawnPoints[i].toFloat();
        g.drawLine(p1.x, p1.y, p2.x, p2.y, 2.0f);
    }
}

void DrawGrid::mouseDown(const juce::MouseEvent& e)
{
    drawnPoints.clear();
    drawnPoints.push_back(e.getPosition());
    repaint();
}

void DrawGrid::mouseDrag(const juce::MouseEvent& e)
{
    drawnPoints.push_back(e.getPosition());
    repaint();
}

void DrawGrid::mouseUp(const juce::MouseEvent&)
{
    if (onCurveFinished)
        onCurveFinished();
}


std::vector<juce::Point<int>> DrawGrid::getDrawnPoints() const
{
    return drawnPoints;
}
std::vector<float> DrawGrid::getPitchCurve() const
{
    std::vector<float> pitchCurve;
    
    for(auto& point : drawnPoints)
    {
        float y = static_cast<float>(point.y);
        float height = static_cast<float>(getHeight());
        
        float normY = juce::jlimit(0.0f, 1.0f, y / height);
        
        float pitch = juce::jmap(normY, 1.0f, 0.0f, -12.0f, 12.0f);
        pitchCurve.push_back(pitch);
    }
    return pitchCurve;
}
