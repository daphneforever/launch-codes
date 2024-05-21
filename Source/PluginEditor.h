/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/DialKnob.h"
#include "ui/DialLookAndFeel.h"
#include "ui/KeyKnob.h"
#include "ui/ToggleSwitch.h"
#include "ui/TextSlider.h"
#include "ui/EnvelopeView.h"

//==============================================================================
/**
*/
class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&, juce::UndoManager&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::ValueTree& nonParamTree;
    
//    DialLookAndFeel dialLookAndFeel;
    
    ToggleSwitch oscSwitch;
    KeyKnob oscGainKnob;
    TextSlider oscInitalSlider;
    TextSlider oscPeakSlider;
    TextSlider oscSustainSlider;
    TextSlider oscEndSlider;
    TextSlider oscAttackSlider;
    TextSlider oscDecaySlider;
    TextSlider oscReleaseSlider;
    
    ToggleSwitch noiseSwitch;
    KeyKnob noiseGainKnob;
    
    DialKnob verbSizeKnob;
    DialKnob verbDampKnob;
    DialKnob verbWidthKnob;
    DialKnob verbMixKnob;
    
    EnvelopeView oscPitchEnv;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
