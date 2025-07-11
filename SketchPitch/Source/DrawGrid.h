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
#include "CurveTypes.h"


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
    void setPlayheadPhase(float phase);
    void setPitchCurveReference(const std::vector<CurvePoint>* externalCurve);
    bool pointInErasedRegion(float xNorm, float pitch) const;


    const std::vector<ErasedRegion>& getErasedRegions() const { return erasedRegions; };

    void reset();
    juce::Point<int> getClampedPoint(juce::Point<int> p) const;

    std::vector<CurvePoint> getPitchCurve() const;
    void setPitchCurve(const std::vector<CurvePoint>& newCurve);

    void setMode(DrawMode mode);

    std::function<void()> onCurveFinished;
    std::function<void()> onErased;
    
    explicit DrawGrid(SharedImages& sharedImages);
    
    void setResampledCurve(const std::vector<CurvePoint>& resampled);
    
    std::vector<Curve> eraseSegmentsFromCurves(const std::vector<Curve>& inputCurves,
                                                      const std::vector<NormalizedPoint>& stroke,
                                                      float tolerance,
                                                      std::vector<ErasedRegion>* erasedOut,
                                                      int width,
                                                      int height);



private:
    
    std::vector<Curve> curves;
    std::vector<Curve> erasedCurves;
    std::vector<NormalizedPoint> currentEraserStroke;
    std::vector<Curve> curvesBeforeEraser;
    std::vector<CurvePoint> resampledCurve;

    Curve currentCurve;

    DrawMode currentMode = DrawMode::Solo;
    
    juce::MouseCursor soloCursor;
    juce::MouseCursor multiCursor;
    juce::MouseCursor eraserMouseCursor;
    
    juce::Point<float> eraserCursor;
    std::vector<NormalizedPoint> eraserPoints;
    std::vector<ErasedRegion> erasedRegions;
    
    std::vector<CurvePoint> fullPitchCurve;

    bool pointNearEraser(const juce::Point<float>& pt, float tolerance, const std::vector<NormalizedPoint>& stroke) const;

    Curve buildCurveFromPoints(const std::vector<CurvePoint>& points);
    
    float playheadPhase = -1.0f;
    const std::vector<CurvePoint>* pitchCurveRef = nullptr;


    SharedImages& sharedImages;
    
};
