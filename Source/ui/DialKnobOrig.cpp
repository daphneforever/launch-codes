/*
  ==============================================================================

    DialKnobOrig.cpp
    Created: 10 May 2024 12:16:57pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DialKnobOrig.h"
#include "MyColors.h"

//==============================================================================
DialKnobOrig::DialKnobOrig(juce::RangedAudioParameter& param, juce::UndoManager* um)
    : audioParam (param)
    , paramAttachment (audioParam, [&] (float v) { updateValue (v); }, um)
{
    setRepaintsOnMouseActivity (true);
    
    paramAttachment.sendInitialUpdate();
}

DialKnobOrig::~DialKnobOrig()
{
}

void DialKnobOrig::paint (juce::Graphics& g)
{
    const auto center = getLocalBounds().getCentre();
    const float toAngle = startAngle + value * (endAngle - startAngle);
    
    const float innerRadius = getWidth() / 2.0f - 10.0f;
    const float outerRadius = getWidth() / 2.0f - 3.0f;
    
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

void DialKnobOrig::resized() {}

void DialKnobOrig::mouseDown (const juce::MouseEvent& e)
{
    paramAttachment.beginGesture();

    e.source.enableUnboundedMouseMovement (true);

    mousePosWhenLastDragged = e.position;
}

void DialKnobOrig::mouseDrag (const juce::MouseEvent& e)
{
    auto diffY = (mousePosWhenLastDragged.y - e.position.y) * sensitivity;

    if (e.mods.isShiftDown())
        diffY *= 0.1f;

    value = juce::jlimit (0.0f, 1.0f, value + diffY);
    const auto newDenormValue = audioParam.convertFrom0to1 (value);
    paramAttachment.setValueAsPartOfGesture (newDenormValue);

    mousePosWhenLastDragged = e.position;
}

void DialKnobOrig::mouseUp (const juce::MouseEvent& e)
{
    paramAttachment.endGesture();

    e.source.enableUnboundedMouseMovement (false);

    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition (e.source.getLastMouseDownPosition());
}

void DialKnobOrig::updateValue(float newValue)
{
    value = audioParam.convertTo0to1(newValue);
    repaint();
}
