/*
  ==============================================================================

    EnvelopeView.h
    Created: 17 May 2024 9:15:34pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"
#include "EnvelopeNode.h"

//==============================================================================
/*
*/
class EnvelopeView : public juce::Component
{
public:
    EnvelopeView(juce::RangedAudioParameter& param1Y,
                 juce::RangedAudioParameter& param2X,
                 juce::RangedAudioParameter& param2Y,
                 juce::RangedAudioParameter& param3X,
                 juce::RangedAudioParameter& param3Y,
                 juce::RangedAudioParameter& param4X,
                 juce::RangedAudioParameter& param4Y,
                 juce::UndoManager* um = nullptr);
    ~EnvelopeView() override;
    
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void resizeXScale();

private:
    void positionNodes();
    EnvelopeNode* getNode(const juce::String&);
    
    void updateNode1Y(float newVal);
    void updateNode2X(float newVal);
    void updateNode2Y(float newVal);
    void updateNode3X(float newVal);
    void updateNode3Y(float newVal);
    void updateNode4X(float newVal);
    void updateNode4Y(float newVal);
    
    juce::RangedAudioParameter& audioParam1Y;
    juce::RangedAudioParameter& audioParam2X;
    juce::RangedAudioParameter& audioParam2Y;
    juce::RangedAudioParameter& audioParam3X;
    juce::RangedAudioParameter& audioParam3Y;
    juce::RangedAudioParameter& audioParam4X;
    juce::RangedAudioParameter& audioParam4Y;
    
    juce::ParameterAttachment paramAttachment1Y;
    juce::ParameterAttachment paramAttachment2X;
    juce::ParameterAttachment paramAttachment2Y;
    juce::ParameterAttachment paramAttachment3X;
    juce::ParameterAttachment paramAttachment3Y;
    juce::ParameterAttachment paramAttachment4X;
    juce::ParameterAttachment paramAttachment4Y;
    
    juce::Rectangle<float> bounds;
    juce::Rectangle<float> mainArea;
    float timeParameterWidth;
    
    const int numXGridLines { 11 };
    const int numYGridLines { 5 };
    const float gridLineWidth { 1.0f };
    
//    juce::Point<float> lastMousePosition;
//    const float dragSensitivity { 0.01f };
//    const float fineDragSensitivity { 0.001f };
    
    EnvelopeNode node1;
    EnvelopeNode node2;
    EnvelopeNode node3;
    EnvelopeNode node4;
    const float nodeSize { 10.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeView)
};
