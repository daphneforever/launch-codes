/*
  ==============================================================================

    ToggleSwitch.h
    Created: 10 May 2024 12:18:35pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"
#include "FocusBorder.h"

//==============================================================================
/*
*/
class ToggleSwitch  : public juce::Component
{
public:
    ToggleSwitch(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    
    bool getState();
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    
    bool keyPressed (const juce::KeyPress& key) override;
    
    void focusGained (FocusChangeType) override;
    void focusLost   (FocusChangeType) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateState (bool newState);
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    
    FocusBorder focusBorder;
    
    bool state;
    
    juce::Colour switchColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleSwitch)
};
