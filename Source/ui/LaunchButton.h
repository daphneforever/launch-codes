/*
  ==============================================================================

    LaunchButton.h
    Created: 10 May 2024 12:20:22pm
    Author:  Daphne Wilkerson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class LaunchButton  : public juce::Component
{
public:
    LaunchButton();
    ~LaunchButton() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LaunchButton)
};
