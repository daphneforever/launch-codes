/*
  ==============================================================================

    FocusBorder.h
    Created: 17 May 2024 1:59:20pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FocusBorder final
{
public:
    
    void drawBorder(const juce::Rectangle<float>& bounds)
    {
        const auto area = bounds.reduced (borderWidth * 0.5f);
        auto radianRotate = 0.0f;

        for (const juce::Point<float> corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
        {
            const float length = 5.0f;
            juce::Path p;
            p.startNewSubPath (corner.x, corner.y + length);
            p.lineTo (corner);
            p.lineTo (corner.x + length, corner.y);
            p.applyTransform (juce::AffineTransform::rotation (radianRotate, corner.x, corner.y));
            borderPath.addPath (p);

            radianRotate += juce::MathConstants<float>::halfPi;
        }
    }
    
    juce::Path& getPath()
    {
        return borderPath;
    }
    
    const float getWidth()
    {
        return borderWidth;
    }
    
private:
    juce::Path borderPath;
    const float borderWidth {1.0f};
};



