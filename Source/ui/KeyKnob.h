/*
  ==============================================================================

    KeyKnob.h
    Created: 10 May 2024 12:17:59pm
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
class KeyKnob final : public juce::Component
{
public:
    KeyKnob(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    
    void mouseDown        (const juce::MouseEvent& event) override;
    void mouseDrag        (const juce::MouseEvent& event) override;
    void mouseUp          (const juce::MouseEvent& event) override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;
    
    bool keyPressed (const juce::KeyPress& key) override;
    
    void focusGained (FocusChangeType) override;
    void focusLost   (FocusChangeType) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateValue (const float);
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    
    float currentValue {};
    
    juce::Point<float> lastMousePosition;
    const float dragSensitivity { 0.01f };
    const float fineDragSensitivity { 0.001f };
    
    FocusBorder focusBorder;
    
    juce::Rectangle<float> mainArea;
    
    const float gaugeWidth { 5.0f };
    const int numGaugeTicks { 15 };
    const float tickWidth { 2.0f };
    
    const float startAngle { 5 * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 11.0f * juce::MathConstants<float>::pi / 4.0f };
    
    juce::Label label;
    TextBox textBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeyKnob)
};
