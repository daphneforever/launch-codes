/*
  ==============================================================================

    WaveshaperRadar.h
    Created: 10 May 2024 12:19:28pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveshaperRadar  : public juce::Component
{
public:
    WaveshaperRadar();
    ~WaveshaperRadar() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveshaperRadar)
};
