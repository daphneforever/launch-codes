/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p, juce::UndoManager& um)
: AudioProcessorEditor (&p), audioProcessor (p)
, apvts(p.getPluginState())
, oscSwitch      (*apvts.getParameter(ParamIDs::oscIO),      &um)
, oscGainKnob    (*apvts.getParameter(ParamIDs::oscGain),    &um)
, oscInitalSlider(*apvts.getParameter(ParamIDs::oscInitial), &um)
, oscPeakSlider  (*apvts.getParameter(ParamIDs::oscPeak), &um)
, oscSustainSlider(*apvts.getParameter(ParamIDs::oscSustain), &um)
, oscEndSlider   (*apvts.getParameter(ParamIDs::oscEnd), &um)
, oscAttackSlider(*apvts.getParameter(ParamIDs::oscAttack), &um)
, oscDecaySlider (*apvts.getParameter(ParamIDs::oscDecay), &um)
, oscReleaseSlider(*apvts.getParameter(ParamIDs::oscRelease), &um)
, noiseSwitch    (*apvts.getParameter(ParamIDs::noiseIO),    &um)
, noiseGainKnob  (*apvts.getParameter(ParamIDs::noiseGain),  &um)
, verbSizeKnob   (*apvts.getParameter(ParamIDs::size),       &um)
, verbDampKnob   (*apvts.getParameter(ParamIDs::damp),       &um)
, verbWidthKnob  (*apvts.getParameter(ParamIDs::width),      &um)
, verbMixKnob    (*apvts.getParameter(ParamIDs::verbMix),    &um)
, oscPitchEnv(*apvts.getParameter(ParamIDs::oscInitial), *apvts.getParameter(ParamIDs::oscAttack), *apvts.getParameter(ParamIDs::oscPeak), *apvts.getParameter(ParamIDs::oscDecay), *apvts.getParameter(ParamIDs::oscSustain), *apvts.getParameter(ParamIDs::oscRelease), *apvts.getParameter(ParamIDs::oscEnd), &um)
    
{
    setSize (400, 550);
    
//    addAndMakeVisible(sizeKnobOrig);
//
//    addAndMakeVisible(knobTest);
//    knobTest.setSliderStyle(juce::Slider::RotaryVerticalDrag);
//    knobTest.setHasFocusOutline(true);
//    knobTest.setWantsKeyboardFocus(true);
//    knobTest.setRotaryParameters(3.0f * juce::MathConstants<float>::pi / 4.0f, 9.0f * juce::MathConstants<float>::pi / 4.0f, true);
////    knobTest.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
//    knobTest.setRange(juce::Range<double> (0.0, 1.0), 0.01);
//    knobTest.setLookAndFeel(&dialLookAndFeel);
    
    addAndMakeVisible(oscSwitch);
    addAndMakeVisible(oscGainKnob);
    addAndMakeVisible(oscInitalSlider);
    addAndMakeVisible(oscPeakSlider);
    addAndMakeVisible(oscSustainSlider);
    addAndMakeVisible(oscEndSlider);
    addAndMakeVisible(oscAttackSlider);
    addAndMakeVisible(oscDecaySlider);
    addAndMakeVisible(oscReleaseSlider);
    
    addAndMakeVisible(oscPitchEnv);
    
    addAndMakeVisible(noiseSwitch);
    addAndMakeVisible(noiseGainKnob);
    
    addAndMakeVisible(verbSizeKnob);
    addAndMakeVisible(verbDampKnob);
    addAndMakeVisible(verbWidthKnob);
    addAndMakeVisible(verbMixKnob);
    
}

PluginEditor::~PluginEditor()
{
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (MyColors::slate);
}

void PluginEditor::resized()
{
    juce::Rectangle<int> knobBounds (10, 10, 70, 70);
    juce::Rectangle<int> switchBounds (10, 170, 35, 50);
    juce::Rectangle<int> textSliderBounds (10, 230, 40, 25);
    juce::Rectangle<int> envelopeBounds (10, 310, 180, 100);
    
    verbSizeKnob .setBounds(knobBounds);
    verbDampKnob .setBounds(knobBounds.withX(90));
    verbWidthKnob.setBounds(knobBounds.withX(170));
    verbMixKnob  .setBounds(knobBounds.withX(250));
    
    oscGainKnob  .setBounds(knobBounds.withY(90));
    noiseGainKnob.setBounds(knobBounds.withX(90).withY(90));
    
    oscSwitch    .setBounds(switchBounds);
    noiseSwitch  .setBounds(switchBounds.withX(55));
    
    oscInitalSlider.setBounds(textSliderBounds);
    oscPeakSlider.setBounds(textSliderBounds.withX(50));
    oscSustainSlider.setBounds(textSliderBounds.withX(90));
    oscEndSlider.setBounds(textSliderBounds.withX(130));
    oscAttackSlider.setBounds(textSliderBounds.withX(50).withY(270));
    oscDecaySlider.setBounds(textSliderBounds.withX(90).withY(270));
    oscReleaseSlider.setBounds(textSliderBounds.withX(130).withY(270));
    
    oscPitchEnv.setBounds(envelopeBounds);
}
