/*
  ==============================================================================

    DialLookAndFeel.h
    Created: 12 May 2024 4:36:49pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"

class DialLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DialLookAndFeel ()
    {
        setColour(juce::Slider::textBoxTextColourId, MyColors::lightGrey);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float value, float startAngle, float endAngle, juce::Slider& slider) override
    {
        const auto area = juce::Rectangle<float> (x, y, width, height);
//        createBorderPath(area);
//
//        g.setColour(MyColors::lightGrey);
//        g.strokePath(borderPath, juce::PathStrokeType(tickWidth));
        
        const auto center = area.getCentre();
        const float toAngle = startAngle + value * (endAngle - startAngle);
        
        const float innerRadius = width / 2.0f - 10.0f;
        const float outerRadius = width / 2.0f - 3.0f;
        
        const int numColoredTicks = juce::jmin((int) (value * numTicks) + 1, numTicks);
        const int numUncoloredTicks = numTicks - numColoredTicks;
        const float coloredEndAngle = startAngle + (float) ((float) numColoredTicks / (numTicks)) * (endAngle - startAngle);
        
        juce::Path levelMarks;
        
        for (int i = 0; i < numColoredTicks; i++)
        {
            
            const float angle = startAngle + ((float) i / (numColoredTicks)) * (coloredEndAngle - startAngle);
            juce::Point<float> innerPoint (center.x + innerRadius * cosf(angle),
                                           center.y + innerRadius * sinf(angle));
            juce::Point<float> outerPoint (center.x + outerRadius * cosf(angle),
                                           center.y + outerRadius * sinf(angle));;
            levelMarks.startNewSubPath(innerPoint);
            levelMarks.lineTo(outerPoint);
        }
        
        g.setColour(juce::Colour(0.27f - value * 0.27f, 0.99f, 0.88f, 1.0f));
        g.strokePath(levelMarks, juce::PathStrokeType(tickWidth));
        levelMarks.clear();
        
        for (int i = 0; i < numUncoloredTicks; i++)
        {
            const float angle = coloredEndAngle + ((float) i / (numUncoloredTicks)) * (endAngle - coloredEndAngle);
            juce::Point<float> innerPoint (center.x + innerRadius * cosf(angle),
                                           center.y + innerRadius * sinf(angle));
            juce::Point<float> outerPoint (center.x + outerRadius * cosf(angle),
                                           center.y + outerRadius * sinf(angle));;
            levelMarks.startNewSubPath(innerPoint);
            levelMarks.lineTo(outerPoint);
        }
        
        g.setColour(MyColors::lightGrey);
        g.strokePath(levelMarks, juce::PathStrokeType(tickWidth));
        
        juce::Path needle;
        needle.startNewSubPath(center.x, center.y + 5.0f);
        needle.lineTo(center.x + innerRadius, center.y);
        needle.lineTo(center.x, center.y - 5.0f);
        needle.lineTo(center.x - 7.0f, center.y - 2.0f);
        needle.lineTo(center.x - 7.0f, center.y + 2.0f);
        needle.closeSubPath();
        g.fillPath(needle, juce::AffineTransform::rotation(toAngle, center.x, center.y));
        
        
        
    }
    
    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider) override
    {
        juce::Slider::SliderLayout layout;
        auto localBounds = slider.getLocalBounds();
        int labelWidth = 40;
        int labelHeight = 15;
        
        layout.sliderBounds = localBounds;
//        layout.textBoxBounds = (localBounds.removeFromBottom(15)).;
        layout.textBoxBounds = juce::Rectangle<int>((localBounds.getWidth() - labelWidth) / 2, localBounds.getHeight() - labelHeight, labelWidth, labelHeight);
        
        return layout;
    }
    
//    juce::Label* createSliderTextBox (juce::Slider& s) override
//    {
//        juce::Label* l = new juce::Label ({}, {});
//        l->
//        return nullptr;
//    };
    
    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll (label.findColour (juce::Label::backgroundColourId));

        if (! label.isBeingEdited())
        {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const juce::Font font (getLabelFont (label));

            g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (font);

            auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

            g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                              juce::jmax (1, (int) ((float) textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());

            g.setColour (label.findColour (juce::Label::outlineColourId).withMultipliedAlpha (alpha));
        }
        else if (label.isEnabled())
        {
            g.setColour (label.findColour (juce::Label::outlineColourId));
        }

//        g.drawRect (label.getLocalBounds());
    }
    
//    void createBorderPath (const juce::Rectangle<float> area)
//    {
//        auto radian = 0.0f;
//
//        for (const juce::Point<float> corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
//        {
//            constexpr float length = 5.0f;
//            juce::Path p;
//            p.startNewSubPath (corner.x, corner.y + length);
//            p.lineTo (corner);
//            p.lineTo (corner.x + length, corner.y);
//            p.applyTransform (juce::AffineTransform::rotation (radian, corner.x, corner.y));
//            borderPath.addPath (p);
//
//            radian += juce::MathConstants<float>::halfPi;
//        };
//    }
    
    std::unique_ptr<juce::FocusOutline> createFocusOutlineForComponent (juce::Component&) override
    {
        struct WindowProperties final : public juce::FocusOutline::OutlineWindowProperties
        {
            juce::Rectangle<int> getOutlineBounds (juce::Component& c) override
            {
                return c.getScreenBounds();
            }

            void drawOutline (juce::Graphics& g, int width, int height) override
            {
//                g.setColour (juce::Colours::yellow.withAlpha (0.6f));
//                g.drawRoundedRectangle ({ (float) width, (float) height }, 3.0f, 3.0f);
                
                juce::Path borderPath;
                
                auto radian = 0.0f;

                const auto area = juce::Rectangle<float> (width, height);
                for (const juce::Point<float> corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
                {
                    constexpr float length = 5.0f;
                    juce::Path p;
                    p.startNewSubPath (corner.x, corner.y + length);
                    p.lineTo (corner);
                    p.lineTo (corner.x + length, corner.y);
                    p.applyTransform (juce::AffineTransform::rotation (radian, corner.x, corner.y));
                    borderPath.addPath (p);

                    radian += juce::MathConstants<float>::halfPi;
                }
                
                g.setColour(MyColors::lightGrey);
                g.strokePath(borderPath, juce::PathStrokeType(3.0f)); // change
                
            }
        };

        return std::make_unique<juce::FocusOutline> (std::make_unique<WindowProperties>());
    }

private:
//    juce::Path borderPath;
    const int numTicks { 20 };
    const float tickWidth { 3.0f };
};
