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
    g.fillAll(juce::Colours::darkslateblue);

    g.setColour(juce::Colours::darkblue);
    for (int x = 0; x < getWidth(); x += cellSize)
        g.drawLine((float)x, 0.f, (float)x, (float)getHeight());

    for (int y = 0; y < getHeight(); y += cellSize)
        g.drawLine(0.f, (float)y, (float)getWidth(), (float)y);

    g.setColour(juce::Colours::lightcyan);
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

    auto clampedPos = getClampedPoint(e.getPosition());
    drawnPoints.push_back(clampedPos);
    repaint();
}

void DrawGrid::mouseDrag(const juce::MouseEvent& e)
{
    auto clampedPos = getClampedPoint(e.getPosition());
    drawnPoints.push_back(clampedPos);
    repaint();
}

juce::Point<int> DrawGrid::getClampedPoint(juce::Point<int> p) const
{
    auto bounds = getLocalBounds();
    int clampedX = juce::jlimit(bounds.getX(), bounds.getRight() - 1, p.getX());
    int clampedY = juce::jlimit(bounds.getY(), bounds.getBottom() - 1, p.getY());
    return { clampedX, clampedY };
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
