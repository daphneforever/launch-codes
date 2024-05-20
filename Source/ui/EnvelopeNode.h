/*
  ==============================================================================

    EnvelopeNode.h
    Created: 17 May 2024 9:16:49pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyColors.h"

//==============================================================================
/*
*/
class EnvelopeNode  : public juce::Component
{
public:
    EnvelopeNode(juce::RangedAudioParameter* paramX,
                 juce::RangedAudioParameter* paramY,
                 juce::ParameterAttachment* attachX,
                 juce::ParameterAttachment* attachY)
    : audioParamX(paramX)
    , audioParamY(paramY)
    , paramAttachmentX(attachX)
    , paramAttachmentY(attachY)
    {
        setTitle("node");
    }

    ~EnvelopeNode() override
    {
    }
    
    void mouseDown (const juce::MouseEvent& event) override
    {
        paintClicked = true;
        repaint();
    }
    
    void mouseUp (const juce::MouseEvent& event) override
    {
        paintClicked = false;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        if (paintClicked)
            g.setColour(MyColors::red);
        else
            g.setColour(MyColors::lightGreen);
        
        g.drawRect(getLocalBounds());
    }

    void resized() override
    {

    }
    
    juce::Point<float> pos;
    juce::Point<float> dragStartPos;
    
    juce::RangedAudioParameter* audioParamX;
    juce::RangedAudioParameter* audioParamY;
    juce::ParameterAttachment* paramAttachmentX;
    juce::ParameterAttachment* paramAttachmentY;

private:
    bool paintClicked {false};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeNode);
};
