/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::NormalisableRange<float> normRangeWithMidpointSkew (const float lowerBound, const float upperBound, const float interval, const float midPoint)
{
    juce::NormalisableRange<float> range (lowerBound, upperBound, interval);
    range.setSkewForCentre(midPoint);
    return range;
}

//==============================================================================
static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto percentFormat = [] (float value, int /*maximumStringLength*/)
    {
        if (value < 10.0f)
            return juce::String { value, 2 } + "%";

        if (value < 100.0f)
            return juce::String { value, 1 } + "%"; // std::floor (value * 10.0f) / 10.0f

        return juce::String { value, 0 } + "%";
    };
    
    auto toDecibelFormat = [] (float value, int /*maximumStringLength*/)
    {
        if (value <= 0.01f)
            return juce::String {"-inf db"};
        
        return juce::String { juce::Decibels::gainToDecibels( value ), 1 } + " db";
    };
    
    auto fromDecibelFormat = [] (const juce::String& text)
    {
        const float db = text.getFloatValue();
        return juce::Decibels::decibelsToGain(db);
    };
    
    auto toPitchFormat = [] (float value, int /*maximumStringLength*/)
    {
        if (value < 0.0f)
            return juce::String { value , 1 } + " st";
        return "+" + juce::String { value , 1 } + " st";
    };
    
//    auto fromPitchFormat = [] (const juce::String& text)
//    {
//        const float st = text.getFloatValue();
//        return juce::Decibels::decibelsToGain(db);
//    };
    
    auto toDurationFormat = [] (float value, int /*maximumStringLength*/)
    {
        if (value < 1.0f)
            return juce::String { value * 1000.f , 0 } + " ms";
        return juce::String { value, 1 } + " s";
    };
    
    // TODO handle typing duration in ms format
    
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::oscIO, 1 }, ParamIDs::oscIO, false));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscGain, 1 },
                                                             ParamIDs::oscGain,
                                                             juce::NormalisableRange { 0.0f, 2.0f, 0.01f, 1.0f },
                                                             1.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toDecibelFormat,
                                                             fromDecibelFormat));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscInitial, 1 },
                                                             "Initial",
                                                             juce::NormalisableRange { -24.0f, 72.0f, 0.1f, 1.0f },
                                                             0.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toPitchFormat,
                                                             nullptr));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscPeak, 1 },
                                                             "Peak",
                                                             juce::NormalisableRange { -24.0f, 72.0f, 0.1f, 1.0f },
                                                             24.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toPitchFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscSustain, 1 },
                                                             "Sustain",
                                                             juce::NormalisableRange { -24.0f, 72.0f, 0.1f, 1.0f },
                                                             0.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toPitchFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscEnd, 1 },
                                                             "End",
                                                             juce::NormalisableRange { -24.0f, 72.0f, 0.1f, 1.0f },
                                                             -12.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toPitchFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscAttack, 1 },
                                                             "Attack",
                                                             normRangeWithMidpointSkew(0.001f, 3.0f, 0.001f, 0.75),
                                                             0.05f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toDurationFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscDecay, 1 },
                                                             "Decay",
                                                             normRangeWithMidpointSkew(0.005f, 3.0f, 0.001f, 0.75),
                                                             1.5f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toDurationFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::oscRelease, 1 },
                                                             "Release",
                                                             normRangeWithMidpointSkew(0.005f, 3.0f, 0.001f, 0.75),
                                                             1.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toDurationFormat,
                                                             nullptr));
    
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::noiseIO, 1 }, ParamIDs::noiseIO, false));
    
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::noiseGain, 1 },
                                                             ParamIDs::noiseGain,
                                                             juce::NormalisableRange { 0.0f, 2.0f, 0.01f, 1.0f },
                                                             1.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             toDecibelFormat,
                                                             fromDecibelFormat));
    
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::verbIO, 1 }, ParamIDs::verbIO, false));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::size, 1 },
                                                             ParamIDs::size,
                                                             juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                             50.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::damp, 1 },
                                                             ParamIDs::damp,
                                                             juce::NormalisableRange { 0.0f, 100.0f, 0.1f, 1.0f },
                                                             50.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::width, 1 },
                                                             ParamIDs::width,
                                                             juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                             50.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ParamIDs::verbMix, 1 },
                                                             ParamIDs::verbMix,
                                                             juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                             50.0f,
                                                             juce::String {},
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             percentFormat,
                                                             nullptr));

    return layout;
}

PluginProcessor::PluginProcessor()

     : AudioProcessor (BusesProperties()
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
     , apvts(*this, &undoManager, "Parameters", createParameterLayout())
{
//    auto castParameter = [&apvts = this->apvts]<typename T> (juce::StringRef paramID, T& destination)
//    {
//        destination = dynamic_cast<T> (apvts.getParameter (paramID));
//        jassert (destination != nullptr);
//    };
//
//    castParameter (ParamIDs::size, size);
//    castParameter (ParamIDs::damp, damp);
//    castParameter (ParamIDs::width, width);
//    castParameter (ParamIDs::mix, mix);
//    castParameter (ParamIDs::verbIO, verbIO);
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this, undoManager);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState& PluginProcessor::getPluginState() { return apvts; }

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
