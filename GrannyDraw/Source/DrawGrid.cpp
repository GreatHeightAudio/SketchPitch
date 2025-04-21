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
    g.fillAll(juce::Colour(0xfffdf5e6));
    
    const int numHorDivs = 12;
    const int numVertDivs = 16;
    
    float w = (float)getWidth();
    float h = (float)getHeight();

    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    for (int i = 0; i <= numVertDivs; ++i)
        {
            float x = w * (float)i / (float)numVertDivs;
            g.drawLine(x, 0.0f, x, h);
        }

        for (int i = 0; i <= numHorDivs; ++i)
        {
            float y = h * (float)i / (float)numHorDivs;
            g.drawLine(0.0f, y, w, y);
        }

    g.setColour(juce::Colour(0xff3b3b3b).withAlpha(0.9f));
    for (size_t i = 1; i < drawnPoints.size(); ++i)
    {
        auto p1 = drawnPoints[i - 1].toFloat();
        auto p2 = drawnPoints[i].toFloat();
        g.drawLine(p1.x, p1.y, p2.x, p2.y, 2.0f);
    }
    
    
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    float centerY = getHeight() / 2.0f;
    g.drawLine(0.f, centerY, (float)getWidth(), centerY, 2.0f);

    g.setColour(juce::Colours::black);
    g.setFont(12.0f);

    for (int i = -12; i <= 12; i += 4)
    {
        float normalizedY = juce::jmap((float)i, -12.0f, 12.0f, 1.0f, 0.0f);
        float y = normalizedY * getHeight();
        g.drawText(juce::String(i), 2, (int)y - 6, 30, 12, juce::Justification::bottom);
        g.drawLine(0.f, y, 6.f, y);
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


std::vector<float> DrawGrid::getPitchCurve() const
{
    std::vector<float> pitchCurve;
    
    for (auto& point : drawnPoints)
    {
        float y = static_cast<float>(point.y);
        float height = static_cast<float>(getHeight());

        float normalizedY = 1.0f - juce::jlimit(0.0f, 1.0f, y / height);

        float pitch = juce::jmap(normalizedY, -12.0f, 12.0f);
        pitch = juce::jlimit(-12.0f, 12.0f, pitch);

        pitchCurve.push_back(pitch);
    }

    
    return pitchCurve;
}

