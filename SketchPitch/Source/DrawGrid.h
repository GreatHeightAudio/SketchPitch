/*
  ==============================================================================

    DrawGrid.h
    Created: 26 Mar 2025 3:22:15pm
    Author:  Samuel Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SharedImages.h"

struct CurvePoint
{
    float normalizedX; // 0.0 to 1.0 (left to right)
    float pitch;       // -12.0 to +12.0 (bottom to top)
};

class DrawGrid : public juce::Component
{
public:
    enum class DrawMode { Solo, Layer, Erase };

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void applyVisualEraser();
    std::vector<CurvePoint> getFullPitchCurve() const;
    void applyLiveEraser();
    void setPlayheadIndex(int index);
    void setPitchCurveReference(const std::vector<CurvePoint>* externalCurve);


    const std::vector<std::pair<float, float>>& getErasedRanges() const { return erasedRanges; }

    void reset();
    juce::Point<int> getClampedPoint(juce::Point<int> p) const;

    std::vector<CurvePoint> getPitchCurve() const;
    void setPitchCurve(const std::vector<CurvePoint>& newCurve);

    void setMode(DrawMode mode);

    std::function<void()> onCurveFinished;
    std::function<void()> onErased;
    
    explicit DrawGrid(SharedImages& sharedImages);


private:
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


    std::vector<Curve> curves;
    std::vector<Curve> erasedCurves;
    std::vector<NormalizedPoint> currentEraserStroke;
    std::vector<Curve> curvesBeforeEraser;


    Curve currentCurve;

    DrawMode currentMode = DrawMode::Solo;
    
    juce::MouseCursor soloCursor;
    juce::MouseCursor multiCursor;
    juce::MouseCursor eraserMouseCursor;
    
    juce::Point<float> eraserCursor;
    std::vector<NormalizedPoint> eraserPoints;
    std::vector<std::pair<float, float>> erasedRanges;
    std::vector<CurvePoint> fullPitchCurve;

    bool pointNearEraser(const juce::Point<float>& pt, float tolerance, const std::vector<NormalizedPoint>& stroke) const;

    Curve buildCurveFromPoints(const std::vector<CurvePoint>& points);
    
    int playheadIndex = -1.0f;
    const std::vector<CurvePoint>* pitchCurveRef = nullptr;


    SharedImages& sharedImages;
    
};
