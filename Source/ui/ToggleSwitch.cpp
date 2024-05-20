/*
  ==============================================================================

    ToggleSwitch.cpp
    Created: 10 May 2024 12:18:35pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ToggleSwitch.h"

//==============================================================================
ToggleSwitch::ToggleSwitch(juce::RangedAudioParameter& param, juce::UndoManager* um)
    : audioParam(param)
    , paramAttachment(param, [&] (bool newState) { updateState(newState); }, um)
    , switchColor(MyColors::lightGrey)
{
    setWantsKeyboardFocus(true);
    setRepaintsOnMouseActivity(true);
    
    paramAttachment.sendInitialUpdate();
}

bool ToggleSwitch::getState()
{
    return state;
}

void ToggleSwitch::mouseDown(const juce::MouseEvent &event)
{
    switchColor = MyColors::mediumGrey;
}

void ToggleSwitch::mouseUp(const juce::MouseEvent &event)
{
    switchColor = MyColors::lightGrey;
    if (contains(event.position))
    {
        paramAttachment.setValueAsCompleteGesture(!state);
    }
}

bool ToggleSwitch::keyPressed(const juce::KeyPress &key)
{
    if (key.isKeyCode(' '))
    {
        paramAttachment.setValueAsCompleteGesture(!state);
        return true;
    }
        
    return false;
}

void ToggleSwitch::focusGained (FocusChangeType cause)
{
    repaint();
}

void ToggleSwitch::focusLost (FocusChangeType cause)
{
    repaint();
}

void ToggleSwitch::paint (juce::Graphics& g)
{
    if (hasKeyboardFocus(true))
    {
        g.setColour(MyColors::lightGrey);
        g.strokePath(focusBorder.getPath(), juce::PathStrokeType(focusBorder.getWidth()));
    }
    
    const juce::Rectangle<float> area = getLocalBounds().toFloat();
    const juce::Point<float> center = area.getCentre();
    
    const float outerRadius = 15.0f;
    const float innerRadius = outerRadius * 0.7f;
    
    g.setColour(MyColors::darkGrey);
    g.fillEllipse(center.x - outerRadius, center.y - outerRadius, outerRadius * 2, outerRadius * 2);
    g.setColour(MyColors::slate);
    g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2, innerRadius * 2);
    
    juce::Path switchPath;
    switchPath.startNewSubPath(center.x - 2.5f, center.y + 2.0f);
    switchPath.lineTo(center.x - 4.0f, area.getY() + 4.0f);
    switchPath.lineTo(center.x + 4.0f, area.getY() + 4.0f);
    switchPath.lineTo(center.x + 2.5f, center.y + 2.0f);
    switchPath.closeSubPath();
    switchPath.addEllipse(center.x - 3.85f, area.getY() + 0.9f, 7.7f, 7.7f);
    
    g.setColour(switchColor);
    
    if (state)
    {
        g.fillPath(switchPath);
    }
    else
    {
        g.fillPath(switchPath, juce::AffineTransform::rotation(juce::MathConstants<float>::pi, center.x, center.y));
    }
}

void ToggleSwitch::resized()
{
    const juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    focusBorder.getPath().clear();
    focusBorder.drawBorder(bounds);
}

void ToggleSwitch::updateState(bool newState)
{
    state = newState;
    repaint();
}
