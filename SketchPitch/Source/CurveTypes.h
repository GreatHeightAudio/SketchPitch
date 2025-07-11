/*
  ==============================================================================

    CurveTypes.h
    Created: 19 Jun 2025 2:23:34pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once

struct CurvePoint
{
    float normalizedX;
    float pitch;
};

struct ErasedRegion
{
    float xMin, xMax;
    float yMin, yMax;
};

struct Curve
{
    std::vector<CurvePoint> points;
    juce::Path path;
    bool isErased = false;
};

struct NormalizedPoint {
    float xNorm; // 0.0 to 1.0
    float yNorm; // 0.0 to 1.0
};
