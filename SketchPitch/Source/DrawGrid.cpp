/*
  ==============================================================================

    DrawGrid.cpp
    Created: 26 Mar 2025 3:22:15pm
    Author:  Samuel Smith

  ==============================================================================
*/

#include "DrawGrid.h"


bool DrawGrid::pointNearEraser(const juce::Point<float>& pt, float tolerance, const std::vector<NormalizedPoint>& stroke) const
{
    if (pt.getDistanceFrom(eraserCursor) < tolerance)
        return true;

    if (stroke.size() < 2)
        return false;

    for (size_t i = 1; i < stroke.size(); ++i)
    {
        juce::Point<float> p1(
            stroke[i - 1].xNorm * getWidth(),
            stroke[i - 1].yNorm * getHeight()
        );

        juce::Point<float> p2(
            stroke[i].xNorm * getWidth(),
            stroke[i].yNorm * getHeight()
        );

        juce::Line<float> seg(p1, p2);
        juce::Point<float> closestPoint;
        float dist = seg.getDistanceFromPoint(pt, closestPoint);
        if (dist < tolerance)
            return true;
    }

    return false;
}

DrawGrid::DrawGrid(SharedImages& images) : sharedImages(images)
{
    setOpaque(true);

    const int cWidth = 25;
    const int cHeight = 25;

    auto soloImg  = sharedImages.getSoloCursor().rescaled(cWidth, cHeight, juce::Graphics::highResamplingQuality);
    auto multiImg = sharedImages.getMultiCursor().rescaled(cWidth, cHeight, juce::Graphics::highResamplingQuality);
    auto eraseImg = sharedImages.getEraserCursor().rescaled(cWidth, cHeight, juce::Graphics::highResamplingQuality);

    soloCursor        = juce::MouseCursor(soloImg, 1, 24);
    multiCursor       = juce::MouseCursor(multiImg, 1, 24);
    eraserMouseCursor = juce::MouseCursor(eraseImg, 7, 18);

    setMouseCursor(soloCursor);
}

void DrawGrid::applyVisualEraser()
{
    const float tol = 6.0f;

    std::vector<Curve> newCurves;
    std::vector<std::pair<float, float>> localErasedRanges;

    for (const auto& curve : curves)
    {
        std::vector<CurvePoint> segmentPoints;
        bool first = true;

        for (size_t i = 0; i + 1 < curve.points.size(); ++i)
        {
            const auto& p1 = curve.points[i];
            const auto& p2 = curve.points[i + 1];

            juce::Point<float> pt1(p1.normalizedX * getWidth(), juce::jmap(p1.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f));
            juce::Point<float> pt2(p2.normalizedX * getWidth(), juce::jmap(p2.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f));

            bool segmentErased = pointNearEraser(pt1, tol, currentEraserStroke) || pointNearEraser(pt2, tol, currentEraserStroke);

            if (!segmentErased)
            {
                if (first)
                {
                    segmentPoints.push_back(p1);
                    first = false;
                }
                segmentPoints.push_back(p2);
            }
            else
            {
                if (segmentPoints.size() >= 2)
                    newCurves.push_back(buildCurveFromPoints(segmentPoints));
                segmentPoints.clear();
                first = true;

                float minX = std::min(p1.normalizedX, p2.normalizedX);
                float maxX = std::max(p1.normalizedX, p2.normalizedX);
                localErasedRanges.emplace_back(minX, maxX);
            }
        }

        if (segmentPoints.size() >= 2)
            newCurves.push_back(buildCurveFromPoints(segmentPoints));
    }

    curves = std::move(newCurves);

    if (localErasedRanges.empty() && !currentEraserStroke.empty())
    {
        float minX = 1.0f, maxX = 0.0f;
        for (const auto& pt : currentEraserStroke)
        {
            float x = pt.xNorm;
            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
        }

        if (maxX - minX > 0.0001f)
        {
            localErasedRanges.emplace_back(minX, maxX);
            DBG("⚠️ Erased range from stroke only: [" << minX << ", " << maxX << "]");
        }
    }

    // No merging here — keep visual erasing raw and faithful
    erasedRanges.insert(erasedRanges.end(), localErasedRanges.begin(), localErasedRanges.end());
    eraserPoints.insert(eraserPoints.end(), currentEraserStroke.begin(), currentEraserStroke.end());

    DBG("🧩 Visual erasedRanges size: " << erasedRanges.size());
    for (const auto& r : erasedRanges)
        DBG("🧩 Visual range: [" << r.first << ", " << r.second << "]");

    repaint();

    // Defer pitch curve/audio smoothing to separate backend method
    if (onErased)
        onErased();
}


void DrawGrid::applyLiveEraser()
{
    std::vector<Curve> previewCurves;

    const float tol = 6.0f;

    for (const auto& curve : curvesBeforeEraser)
    {
        std::vector<CurvePoint> segmentPoints;
        bool first = true;

        for (size_t i = 0; i + 1 < curve.points.size(); ++i)
        {
            const auto& p1 = curve.points[i];
            const auto& p2 = curve.points[i + 1];

            juce::Point<float> pt1(p1.normalizedX * getWidth(), juce::jmap(p1.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f));
            juce::Point<float> pt2(p2.normalizedX * getWidth(), juce::jmap(p2.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f));

            bool segmentErased = pointNearEraser(pt1, tol, currentEraserStroke) || pointNearEraser(pt2, tol, currentEraserStroke);

            if (!segmentErased)
            {
                if (first)
                {
                    segmentPoints.push_back(p1);
                    first = false;
                }
                segmentPoints.push_back(p2);
            }
            else
            {
                if (segmentPoints.size() >= 2)
                    previewCurves.push_back(buildCurveFromPoints(segmentPoints));
                segmentPoints.clear();
                first = true;
            }
        }

        if (segmentPoints.size() >= 2)
            previewCurves.push_back(buildCurveFromPoints(segmentPoints));
    }

    curves = std::move(previewCurves);
    repaint();
    
    DBG("[ERASED] erasedRanges size: " << erasedRanges.size());
    for (const auto& r : erasedRanges)
        DBG("[ERASED] Range: [" << r.first << ", " << r.second << "]");

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

    if (!erasedRanges.empty())
    {
        g.setColour(juce::Colours::transparentWhite.withAlpha(0.f));
        for (const auto& ep : eraserPoints)
        {
            float x = ep.xNorm * getWidth();
            float y = ep.yNorm * getHeight();
            g.fillEllipse(x - 1.5f, y - 1.5f, 3.0f, 3.0f);
        }
    }

    // Normal curves
    g.setColour(juce::Colour(0xff3b3b3b).withAlpha(0.9f));
    for (const auto& curve : curves)
        g.strokePath(curve.path, juce::PathStrokeType(2.0f));

    // Erased curves
    g.setColour(juce::Colours::darkred.withAlpha(0.3f));
    for (const auto& curve : erasedCurves)
        g.strokePath(curve.path, juce::PathStrokeType(2.0f));

    if ((currentMode == DrawMode::Solo || currentMode == DrawMode::Layer) &&
        !currentCurve.points.empty())
    {
        g.strokePath(currentCurve.path, juce::PathStrokeType(2.0f));
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

    if (currentMode == DrawMode::Erase)
    {
        const float radius = 6.0f;
        g.setColour(juce::Colours::transparentWhite.withAlpha(0.f));
        g.drawEllipse(eraserCursor.x - radius,
                      eraserCursor.y - radius,
                      radius * 2.0f,
                      radius * 2.0f,
                      2.0f);
    }

    if (playheadIndex >= 0 && pitchCurveRef != nullptr && !pitchCurveRef->empty())
    {
        int clampedIndex = juce::jlimit(0, (int)pitchCurveRef->size() - 1, playheadIndex);
        const auto& pt = (*pitchCurveRef)[clampedIndex];

        float x = pt.normalizedX * getWidth();
        float y = juce::jmap(pt.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);

        g.setColour(juce::Colours::red.withAlpha(0.8f));
        g.fillEllipse(x - 4, y - 4, 8.0f, 8.0f);
    }
}

void DrawGrid::setPlayheadIndex(int index)
{
    playheadIndex = index;
    repaint();
}

void DrawGrid::setPitchCurveReference(const std::vector<CurvePoint>* externalCurve)
{
    pitchCurveRef = externalCurve;
    repaint();
}

void DrawGrid::mouseMove(const juce::MouseEvent& e)
{
    if (currentMode == DrawMode::Erase)
    {
        eraserCursor = e.position;
        repaint();
    }
}

void DrawGrid::mouseDown(const juce::MouseEvent& e)
{
    if (currentMode == DrawMode::Erase)
    {
        currentEraserStroke.clear();
        curvesBeforeEraser = curves;
        NormalizedPoint p;
        p.xNorm = e.position.x / (float)getWidth();
        p.yNorm = e.position.y / (float)getHeight();
        currentEraserStroke.push_back(p);
        repaint();
        return;
    }

    if (currentMode == DrawMode::Solo)
    {
        curves.clear();
        eraserPoints.clear();
        erasedRanges.clear();
        currentCurve.points.clear();
        currentCurve.path.clear();
        fullPitchCurve.clear();
        erasedRanges.clear();
        
        if (onErased)
                onErased();
    }

    auto clampedPos = getClampedPoint(e.getPosition());

    CurvePoint cp;
    cp.normalizedX = clampedPos.x / (float)getWidth();
    cp.pitch = juce::jmap(static_cast<float>(clampedPos.y),
                          0.0f,
                          static_cast<float>(getHeight()),
                          12.0f,
                          -12.0f);

    float x = cp.normalizedX * getWidth();
    float y = juce::jmap(cp.pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);

    currentCurve.path.startNewSubPath(x, y);
    currentCurve.points.push_back(cp);
    repaint();
}

void DrawGrid::mouseDrag(const juce::MouseEvent& e)
{
    if (currentMode == DrawMode::Erase)
        {
            auto lastNorm = currentEraserStroke.back();
            applyLiveEraser();
            juce::Point<float> last(
                lastNorm.xNorm * (float)getWidth(),
                lastNorm.yNorm * (float)getHeight()
            );
            juce::Point<float> current = getClampedPoint(e.getPosition()).toFloat();

            const float step = 2.0f;
            float distance = last.getDistanceFrom(current);
            int steps = std::max(1, static_cast<int>(distance / step));

            for (int i = 1; i <= steps; ++i)
            {
                float alpha = static_cast<float>(i) / static_cast<float>(steps);
                juce::Point<float> interp(
                    last.x + alpha * (current.x - last.x),
                    last.y + alpha * (current.y - last.y)
                );
                NormalizedPoint p;
                p.xNorm = interp.x / (float)getWidth();
                p.yNorm = interp.y / (float)getHeight();
                currentEraserStroke.push_back(p);
            }

            NormalizedPoint p;
            p.xNorm = current.x / (float)getWidth();
            p.yNorm = current.y / (float)getHeight();
            currentEraserStroke.push_back(p);

            eraserCursor = current;
            repaint();
            return;
        }

    // Solo or Layer draw mode
    juce::Point<float> lastPoint = currentCurve.points.empty()
        ? getClampedPoint(e.getPosition()).toFloat()
        : juce::Point<float>(
              currentCurve.points.back().normalizedX * (float)getWidth(),
              juce::jmap(currentCurve.points.back().pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f)
          );

    juce::Point<float> current = getClampedPoint(e.getPosition()).toFloat();

    float dx = current.x - lastPoint.x;
    float dy = current.y - lastPoint.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    const float step = 1.0f;
    int steps = std::max(1, static_cast<int>(distance / step));

    for (int i = 0; i <= steps; ++i)
    {
        float alpha = static_cast<float>(i) / static_cast<float>(steps);
        float x = lastPoint.x + alpha * dx;
        float y = lastPoint.y + alpha * dy;

        CurvePoint cp;
        cp.normalizedX = x / (float)getWidth();
        cp.pitch = juce::jmap(y,
                              0.0f,
                              static_cast<float>(getHeight()),
                              12.0f,
                              -12.0f);

        currentCurve.points.push_back(cp);
        currentCurve.path.lineTo(x, y);
    }

    repaint();
}




void DrawGrid::mouseUp(const juce::MouseEvent&)
{
    if (currentMode == DrawMode::Erase)
    {
        eraserPoints.insert(eraserPoints.end(), currentEraserStroke.begin(), currentEraserStroke.end());
        applyVisualEraser();
        currentEraserStroke.clear();
        repaint();
        return;
    }

    if (!currentCurve.points.empty())
    {
        curves.push_back(currentCurve);
        fullPitchCurve.insert(fullPitchCurve.end(),
                              currentCurve.points.begin(),
                              currentCurve.points.end());
    }

    currentCurve.points.clear();
    currentCurve.path.clear();

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
    std::vector<CurvePoint> allPoints;
    for (const auto& curve : curves)
        allPoints.insert(allPoints.end(), curve.points.begin(), curve.points.end());

    std::sort(allPoints.begin(), allPoints.end(), [](const CurvePoint& a, const CurvePoint& b) {
        return a.normalizedX < b.normalizedX;
    });

    return allPoints;
}


void DrawGrid::setPitchCurve(const std::vector<CurvePoint>& newCurve)
{
    Curve curve;
    curve.points = newCurve;

    if (!newCurve.empty())
    {
        float x = newCurve[0].normalizedX * getWidth();
        float y = juce::jmap(newCurve[0].pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);
        curve.path.startNewSubPath(x, y);

        for (size_t i = 1; i < newCurve.size(); ++i)
        {
            float px = newCurve[i].normalizedX * getWidth();
            float py = juce::jmap(newCurve[i].pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);
            curve.path.lineTo(px, py);
        }

        curves.clear();
        curves.push_back(curve);
        fullPitchCurve = newCurve;
        repaint();
    }
}

DrawGrid::Curve DrawGrid::buildCurveFromPoints(const std::vector<CurvePoint>& points)
{
    Curve c;
    c.points = points;

    if (!points.empty())
    {
        float x = points[0].normalizedX * getWidth();
        float y = juce::jmap(points[0].pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);
        c.path.startNewSubPath(x, y);

        for (size_t i = 1; i < points.size(); ++i)
        {
            float px = points[i].normalizedX * getWidth();
            float py = juce::jmap(points[i].pitch, -12.0f, 12.0f, (float)getHeight(), 0.0f);
            c.path.lineTo(px, py);
        }
    }
    return c;
}

std::vector<CurvePoint> DrawGrid::getFullPitchCurve() const
{
    return fullPitchCurve;
}

void DrawGrid::setMode(DrawMode mode)
{
    currentMode = mode;

    if (currentMode == DrawMode::Solo)
    {
        setMouseCursor(soloCursor);
    }
    else if (currentMode == DrawMode::Layer)
    {
        setMouseCursor(multiCursor);
    }
    else if (currentMode == DrawMode::Erase)
    {
        setMouseCursor(eraserMouseCursor);
    }
    else
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }

    repaint();
}

void DrawGrid::reset()
{
    curves.clear();
    eraserPoints.clear();
    erasedRanges.clear();
    erasedCurves.clear();
    fullPitchCurve.clear();
    currentCurve.points.clear();
    currentCurve.path.clear();
    repaint();
}

void DrawGrid::resized()
{
    for (auto& curve : curves)
        curve = buildCurveFromPoints(curve.points);

    currentCurve = buildCurveFromPoints(currentCurve.points);
    repaint();
}

