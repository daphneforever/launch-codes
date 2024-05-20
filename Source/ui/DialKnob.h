/*
  ==============================================================================

    DialKnob.h
    Created: 16 May 2024 10:33:01am
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
class DialKnob final : public juce::Component
{
public:
    DialKnob(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;
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
    
    const int numTicks { 20 };
    const int numNeedleTicks { 15 };
    const float tickWidth { 2.0f };
    
    const float startAngle { 3.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 9.0f * juce::MathConstants<float>::pi / 4.0f };
    
    juce::Label label;
    TextBox textBox;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialKnob)
};
