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
        g.drawLine(w * i / (float)numVertDivs, 0.0f, w * i / (float)numVertDivs, h);

    for (int i = 0; i <= numHorDivs; ++i)
        g.drawLine(0.0f, h * i / (float)numHorDivs, w, h * i / (float)numHorDivs);

    g.setColour(juce::Colour(0xff3b3b3b).withAlpha(0.9f));

    for (size_t i = 1; i < drawnCurvePoints.size(); ++i)
    {
        auto& p1 = drawnCurvePoints[i - 1];
        auto& p2 = drawnCurvePoints[i];

        float x1 = p1.normalizedX * getWidth();
        float y1 = juce::jmap(p1.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);

        float x2 = p2.normalizedX * getWidth();
        float y2 = juce::jmap(p2.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);

        g.drawLine(x1, y1, x2, y2, 2.0f);
    }

    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    float centerY = getHeight() / 2.0f;
    g.drawLine(0.f, centerY, (float)getWidth(), centerY, 2.0f);

    g.setColour(juce::Colours::black);
    g.setFont(12.0f);

    const int margin = 8;

    for (int i = -12; i <= 12; i += 4)
    {
        float normalizedY = juce::jmap((float)i, -12.0f, 12.0f, 1.0f, 0.0f);
        float y = normalizedY * getHeight();

        y = juce::jlimit(0.0f + margin, (float)getHeight() - margin, y);

        g.drawText(juce::String(i), 2, (int)(y - 6), 40, 12, juce::Justification::centredLeft);

    }
}

void DrawGrid::mouseDown(const juce::MouseEvent& e)
{
    drawnCurvePoints.clear();

    auto clampedPos = getClampedPoint(e.getPosition());

    CurvePoint cp;
    cp.normalizedX = clampedPos.x / (float)getWidth();
    cp.pitch = juce::jmap(1.0f - clampedPos.y / (float)getHeight(), -12.0f, 12.0f);

    drawnCurvePoints.push_back(cp);
    repaint();
}

void DrawGrid::mouseDrag(const juce::MouseEvent& e)
{
    auto clampedPos = getClampedPoint(e.getPosition());

    CurvePoint cp;
    cp.normalizedX = clampedPos.x / (float)getWidth();
    cp.pitch = juce::jmap(1.0f - clampedPos.y / (float)getHeight(), -12.0f, 12.0f);

    drawnCurvePoints.push_back(cp);
    repaint();
}

void DrawGrid::mouseUp(const juce::MouseEvent&)
{
    if (onCurveFinished)
        onCurveFinished();
}

juce::Point<int> DrawGrid::getClampedPoint(juce::Point<int> p) const
{
    auto bounds = getLocalBounds();
    int clampedX = juce::jlimit(bounds.getX(), bounds.getRight() - 1, p.x);
    int clampedY = juce::jlimit(bounds.getY(), bounds.getBottom() - 1, p.y);
    return { clampedX, clampedY };
}

std::vector<CurvePoint> DrawGrid::getPitchCurve() const
{
    return drawnCurvePoints;
}

void DrawGrid::setPitchCurve(const std::vector<CurvePoint>& newCurve)
{
    drawnCurvePoints = newCurve;
    repaint();
}
