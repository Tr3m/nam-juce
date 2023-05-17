#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NamJUCEAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NamJUCEAudioProcessorEditor (NamJUCEAudioProcessor&);
    ~NamJUCEAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)
};
