/*
  ==============================================================================

    DialKnob.cpp
    Created: 16 May 2024 10:33:01am
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DialKnob.h"

//==============================================================================
DialKnob::DialKnob(juce::RangedAudioParameter& param, juce::UndoManager* um)
    : audioParam(param)
    , paramAttachment(param, [&] (float val) {updateValue(val);}, um)
{
    setWantsKeyboardFocus(true);
    setRepaintsOnMouseActivity(true);
    
    addAndMakeVisible(label);
    label.setText(audioParam.getName(10), juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setInterceptsMouseClicks (false, false);
    label.setColour(juce::Label::textColourId, MyColors::white);
    
    addAndMakeVisible(textBox);
    textBox.setColour(juce::Label::textColourId, MyColors::white);
    textBox.setColour(juce::Label::textWhenEditingColourId, MyColors::white);
    
    textBox.onTextChange = [&]
    {
        const float normVal = audioParam.getValueForText(textBox.getText());
        const float denormVal = audioParam.convertFrom0to1(normVal);
        paramAttachment.setValueAsCompleteGesture(denormVal);
        textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    };
    
    paramAttachment.sendInitialUpdate();
}

void DialKnob::mouseDown(const juce::MouseEvent& event)
{
    event.source.enableUnboundedMouseMovement(true);
    paramAttachment.beginGesture();
    lastMousePosition = event.position;
}

void DialKnob::mouseDrag(const juce::MouseEvent& event)
{
    //TODO rewrite with getDistanceFromDragStart
    
    float delta = lastMousePosition.y - event.y;
    event.mods.isShiftDown() ? delta *= fineDragSensitivity : delta *= dragSensitivity;
    
    currentValue = juce::jlimit(0.0f, 1.0f, currentValue + delta);
    const float denormValue = audioParam.convertFrom0to1(currentValue);
    paramAttachment.setValueAsPartOfGesture(denormValue);
    textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    
    lastMousePosition = event.position;
}

void DialKnob::mouseUp(const juce::MouseEvent& event)
{
    event.source.enableUnboundedMouseMovement(false);
    paramAttachment.endGesture();
    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
}

void DialKnob::mouseDoubleClick(const juce::MouseEvent& event)
{
    const float defaultVal = audioParam.getDefaultValue();
    currentValue = defaultVal;
    
    paramAttachment.setValueAsCompleteGesture(audioParam.convertFrom0to1(defaultVal));
    
    textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
}

bool DialKnob::keyPressed(const juce::KeyPress &key)
{
    if (('0' <= key.getKeyCode() && '9' >= key.getKeyCode()) || '.' == key.getKeyCode() || '-' == key.getKeyCode())
    {
        textBox.valueShownWithEditor = juce::String::charToString(key.getKeyCode());
        textBox.showEditor();
        return true;
    }
    
    return false;
}

void DialKnob::focusGained (FocusChangeType cause)
{
    repaint();
}

void DialKnob::focusLost (FocusChangeType cause)
{
    repaint();
}

void DialKnob::paint (juce::Graphics& g)
{
    if (hasKeyboardFocus(true))
    {
        g.setColour(MyColors::lightGrey);
        g.strokePath(focusBorder.getPath(), juce::PathStrokeType(focusBorder.getWidth()));
    }
    
    const auto center = mainArea.getCentre();
    const float toAngle = startAngle + currentValue * (endAngle - startAngle);
    
    const float innerRadius = juce::jmin(mainArea.getWidth(), mainArea.getHeight()) / 2.0f - 9.0f;
    const float outerRadius = juce::jmin(mainArea.getWidth(), mainArea.getHeight()) / 2.0f - 2.0f;
    
    const int numColoredTicks = juce::jmin((int) (currentValue * numTicks) + 1, numTicks);
    
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
    
    g.setColour(juce::Colour(0.27f - currentValue * 0.27f, 0.99f, 0.88f, 1.0f));
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
    needle.startNewSubPath(center.x + innerRadius, center.y);
    needle.lineTo(center.x + innerRadius - 7.0f, center.y);
    
    for (int i = 1; i < numNeedleTicks; i++)
    {
        const float angle = ((float) i / numNeedleTicks) * juce::MathConstants<float>::twoPi;
        juce::Point<float> innerPoint (center.x + (innerRadius - 4.0f) * cosf(angle),
                                       center.y + (innerRadius - 4.0f) * sinf(angle));
        juce::Point<float> outerPoint (center.x + (innerRadius - 2.0f) * cosf(angle),
                                       center.y + (innerRadius - 2.0f) * sinf(angle));;
        needle.startNewSubPath(innerPoint);
        needle.lineTo(outerPoint);
    }
    
    needle.applyTransform(juce::AffineTransform::rotation(toAngle, center.x, center.y));
    g.strokePath(needle, juce::PathStrokeType(tickWidth));
    
//    needle.startNewSubPath(center.x, center.y + 5.0f);
//    needle.lineTo(center.x + innerRadius, center.y);
//    needle.lineTo(center.x, center.y - 5.0f);
//    needle.lineTo(center.x - 7.0f, center.y - 2.0f);
//    needle.lineTo(center.x - 7.0f, center.y + 2.0f);
//    needle.closeSubPath();
//    g.fillPath(needle, juce::AffineTransform::rotation(toAngle, center.x, center.y));
}

void DialKnob::resized()
{
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    focusBorder.getPath().clear();
    focusBorder.drawBorder(bounds);
    
    label.setBounds(bounds.removeFromBottom(bounds.getHeight() / 5.0f).toNearestInt());
    label.setFont(static_cast<float> (label.getHeight()) * 0.9f);
    
    mainArea = bounds.expanded(2.0f).withY(bounds.getY() + 2.0f);
    
    juce::Rectangle<float> textBoxBounds (mainArea.getWidth() - 32.0f, 10.0f);
    textBoxBounds.setCentre(mainArea.getCentre());
    textBox.setBounds(textBoxBounds.toNearestInt());
    textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.9f);
}

void DialKnob::updateValue(const float value)
{
    currentValue = audioParam.convertTo0to1(value);
    textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    repaint();
}
