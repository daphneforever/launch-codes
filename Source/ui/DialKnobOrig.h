/*
  ==============================================================================

    DialKnobOrig.h
    Created: 10 May 2024 12:16:57pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"

//==============================================================================
/*
*/
class DialKnobOrig  : public juce::Component
{
public:
    DialKnobOrig(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    ~DialKnobOrig() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp   (const juce::MouseEvent&) override;

private:
    void updateValue (float);
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    
    float value { 0.6 };
    const float sensitivity { 0.01f };
    float interval { 1.0f };
    float fineInterval { 0.1f };
    
    juce::Point<float> mousePosWhenLastDragged;
    
    const int numTicks { 20 };
    const float tickWidth { 3.0f };
    
    const float startAngle { 3.0f * juce::MathConstants<float>::pi / 4.0f };
    const float endAngle { 9.0f * juce::MathConstants<float>::pi / 4.0f };
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DialKnobOrig)
};
