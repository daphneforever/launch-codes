/*
  ==============================================================================

    TextSlider.h
    Created: 17 May 2024 9:18:13pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"
#include "TextBox.h"
#include "FocusBorder.h"

//==============================================================================
/*
*/
class TextSlider final : public juce::Component
{
public:
    TextSlider(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr)
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
    };
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        event.source.enableUnboundedMouseMovement(true);
        paramAttachment.beginGesture();
        lastMousePosition = event.position;
    };
    
    void mouseDrag (const juce::MouseEvent& event) override
    {
        //TODO rewrite with getDistanceFromDragStart
        
        float delta = lastMousePosition.y - event.y;
        event.mods.isShiftDown() ? delta *= fineDragSensitivity : delta *= dragSensitivity;
        
        currentValue = juce::jlimit(0.0f, 1.0f, currentValue + delta);
        const float denormValue = audioParam.convertFrom0to1(currentValue);
        paramAttachment.setValueAsPartOfGesture(denormValue);
        textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
        
        lastMousePosition = event.position;
    };
    
    void mouseUp   (const juce::MouseEvent& event) override
    {
        event.source.enableUnboundedMouseMovement(false);
        paramAttachment.endGesture();
        juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(event.source.getLastMouseDownPosition());
    };
    
    void mouseDoubleClick (const juce::MouseEvent& event) override
    {
        const float defaultVal = audioParam.getDefaultValue();
        currentValue = defaultVal;
        
        paramAttachment.setValueAsCompleteGesture(audioParam.convertFrom0to1(defaultVal));
        
        textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
    };
    
    bool keyPressed (const juce::KeyPress& key) override
    {
        if (('0' <= key.getKeyCode() && '9' >= key.getKeyCode()) || '.' == key.getKeyCode() || '-' == key.getKeyCode())
        {
            textBox.valueShownWithEditor = juce::String::charToString(key.getKeyCode());
            textBox.showEditor();
            return true;
        }
        
        return false;
    };
    
    void focusGained (FocusChangeType) override
    {
        repaint();
    };
    
    void focusLost (FocusChangeType) override
    {
        repaint();
    };

    void paint (juce::Graphics& g) override
    {
        if (hasKeyboardFocus(true))
        {
            g.setColour(MyColors::lightGrey);
            g.strokePath(focusBorder.getPath(), juce::PathStrokeType(focusBorder.getWidth()));
        }
    };
    
    void resized() override
    {
        juce::Rectangle<float> bounds = getLocalBounds().toFloat();
        
        focusBorder.getPath().clear();
        focusBorder.drawBorder(bounds);
        
        label.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5f).toNearestInt());
        label.setFont(static_cast<float> (label.getHeight()) * 0.9f);
        
        mainArea = bounds;
        textBox.setBounds(mainArea.toNearestInt());
        textBox.setFont(static_cast<float> (textBox.getHeight()) * 0.9f);
    };

private:
    void updateValue (const float value)
    {
        currentValue = audioParam.convertTo0to1(value);
        textBox.setText(audioParam.getCurrentValueAsText(), juce::dontSendNotification);
        repaint();
    };
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    
    float currentValue {};
    
    juce::Point<float> lastMousePosition;
    const float dragSensitivity { 0.01f };
    const float fineDragSensitivity { 0.001f };
    
    FocusBorder focusBorder;
    
    juce::Rectangle<float> mainArea;
    
    const int numTicks { 20 };
    const int numNeedleTicks { 15 };
    const float tickWidth { 2.0f };
    
    const float startAngle { 3.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 9.0f * juce::MathConstants<float>::pi / 4.0f };
    
    juce::Label label;
    TextBox textBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextSlider)
};
