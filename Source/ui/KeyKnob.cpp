/*
  ==============================================================================

    KeyKnob.cpp
    Created: 10 May 2024 12:17:59pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "KeyKnob.h"

//==============================================================================
KeyKnob::KeyKnob(juce::RangedAudioParameter& param, juce::UndoManager* um)
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
    textBox.setColour(juce::Label::textColourId, MyColors::slate);
    textBox.setColour(juce::Label::textWhenEditingColourId, MyColors::slate);
    
    textBox.onTextChange = [&]
    {
        const float normVal = audioParam.getValueForText(textBox.getText());
        const float denormVal = audioParam.convertFrom0to1(normVal);
        paramAttachment.setValueAsCompleteGesture(denormVal);
        textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    };
    
    paramAttachment.sendInitialUpdate();
}

void KeyKnob::mouseDown(const juce::MouseEvent& event)
{
    event.source.enableUnboundedMouseMovement(true);
    paramAttachment.beginGesture();
    lastMousePosition = event.position;
}

void KeyKnob::mouseDrag(const juce::MouseEvent& event)
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

void KeyKnob::mouseUp(const juce::MouseEvent& event)
{
    event.source.enableUnboundedMouseMovement(false);
    paramAttachment.endGesture();
    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
}

void KeyKnob::mouseDoubleClick(const juce::MouseEvent& event)
{
    const float defaultVal = audioParam.getDefaultValue();
    currentValue = defaultVal;
    
    paramAttachment.setValueAsCompleteGesture(audioParam.convertFrom0to1(defaultVal));
    
    textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
}

bool KeyKnob::keyPressed(const juce::KeyPress &key)
{
    if (('0' <= key.getKeyCode() && '9' >= key.getKeyCode()) || '.' == key.getKeyCode() || '-' == key.getKeyCode())
    {
        textBox.valueShownWithEditor = juce::String::charToString(key.getKeyCode());
        textBox.showEditor();
        return true;
    }
    
    return false;
}

void KeyKnob::focusGained (FocusChangeType cause)
{
    repaint();
}

void KeyKnob::focusLost (FocusChangeType cause)
{
    repaint();
}

void KeyKnob::paint (juce::Graphics& g)
{
    if (hasKeyboardFocus(true))
    {
        g.setColour(MyColors::lightGrey);
        g.strokePath(focusBorder.getPath(), juce::PathStrokeType(focusBorder.getWidth()));
    }
    
    const juce::Point<float> center = mainArea.getCentre();
    const float toAngle = startAngle + currentValue * (endAngle - startAngle);
    
    const float baseRadius  = juce::jmin(mainArea.getWidth(), mainArea.getHeight()) / 2.0f;
    const float innerRadius = baseRadius * 0.45f;
    const float outerRadius = baseRadius - 2.0f;
    const float gaugeRadius = baseRadius * 0.75f;
    
    g.setColour(MyColors::lightGrey);
    g.fillEllipse(center.x - outerRadius, center.y - outerRadius, outerRadius * 2, outerRadius * 2);
    
    juce::Path gaugePath;
    gaugePath.addCentredArc(center.x, center.y, gaugeRadius, gaugeRadius, 0.0f, startAngle, endAngle, true);
    g.setColour(MyColors::white);
    g.strokePath(gaugePath, juce::PathStrokeType(gaugeWidth));
    
    gaugePath.clear();
    gaugePath.addCentredArc(center.x, center.y, gaugeRadius, gaugeRadius, 0.0f, startAngle, toAngle, true);
    g.setColour(MyColors::lightGreen);
    g.strokePath(gaugePath, juce::PathStrokeType(gaugeWidth));
    
    juce::Path gaugeTicksPath;
    for (int i = 0; i < numGaugeTicks; i++)
    {
        const float angle = ((float) i / numGaugeTicks) * juce::MathConstants<float>::twoPi;
        juce::Point<float> innerPoint (center.x + (gaugeRadius - 2.5f) * cosf(angle),
                                       center.y + (gaugeRadius - 2.5f) * sinf(angle));
        juce::Point<float> outerPoint (center.x + (gaugeRadius + 2.5f) * cosf(angle),
                                       center.y + (gaugeRadius + 2.5f) * sinf(angle));
        gaugeTicksPath.startNewSubPath(innerPoint);
        gaugeTicksPath.lineTo(outerPoint);
    }
    
    gaugeTicksPath.applyTransform(juce::AffineTransform::rotation(toAngle / 2.0f, center.x, center.y));
    g.setColour(MyColors::lightGrey);
    g.strokePath(gaugeTicksPath, juce::PathStrokeType(tickWidth));
    
    g.setColour(MyColors::white);
    g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2, innerRadius * 2);
    
    juce::Rectangle<float> holeArea (innerRadius * 0.5f, innerRadius * 1.5f);
    holeArea.setCentre(center);
    juce::Path holePath;
    
    holePath.startNewSubPath(holeArea.getBottomRight());
    holePath.lineTo(holeArea.getBottomLeft());
    holePath.lineTo(holeArea.getX(), holeArea.getCentreY());
    holePath.lineTo(holeArea.getCentre());
    holePath.lineTo(holeArea.getCentreX(), holeArea.getY() + 0.35 * holeArea.getHeight());
    holePath.lineTo(holeArea.getX() + 0.15 * holeArea.getWidth(), holeArea.getY() + 0.2 * holeArea.getHeight());
    holePath.lineTo(holeArea.getX() + 0.15 * holeArea.getWidth(), holeArea.getY());
    holePath.lineTo(holeArea.getX() + 0.7 * holeArea.getWidth(), holeArea.getY());
    holePath.lineTo(holeArea.getX() + 0.7 * holeArea.getWidth(), holeArea.getY() + 0.15 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.35 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.6 * holeArea.getHeight());
    holePath.lineTo(holeArea.getCentreX(), holeArea.getY() + 0.8 * holeArea.getHeight());
    holePath.lineTo(holeArea.getRight(), holeArea.getY() + 0.8 * holeArea.getHeight());
    holePath.closeSubPath();
    g.setColour(MyColors::slate);
    
    g.fillPath(holePath, juce::AffineTransform::rotation(toAngle, center.x, center.y));
}

void KeyKnob::resized()
{
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    
    focusBorder.getPath().clear();
    focusBorder.drawBorder(bounds);
    
    label.setBounds(bounds.removeFromBottom(bounds.getHeight() / 5.0f).toNearestInt());
    label.setFont(static_cast<float> (label.getHeight()) * 0.9f);
    
    mainArea = bounds;
    
    juce::Rectangle<float> textBoxBounds (mainArea.getWidth() - 32.0f, 10.0f);
    textBoxBounds.setCentre(mainArea.getCentre().x, mainArea.getCentre().y + juce::jmin(mainArea.getWidth(), mainArea.getHeight()) * 3.0f / 8.0f - 3.0f);
    textBox.setBounds(textBoxBounds.toNearestInt());
    textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.9f);
}

void KeyKnob::updateValue(const float value)
{
    currentValue = audioParam.convertTo0to1(value);
    textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    repaint();
}

