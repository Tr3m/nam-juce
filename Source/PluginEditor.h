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

    enum PluginKnobs
    {
        Input = 0,
        NoiseGate,
        Bass,
        Middle,
        Treble,
        Output
    };

private:

    std::unique_ptr<juce::Slider> sliders[6];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[6];

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)
};
