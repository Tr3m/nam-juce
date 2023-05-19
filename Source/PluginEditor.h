#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

//==============================================================================
/**
*/
class NamJUCEAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer, public juce::Slider::Listener
{
public:
    NamJUCEAudioProcessorEditor (NamJUCEAudioProcessor&);
    ~NamJUCEAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback();
    void sliderValueChanged(juce::Slider* slider);
    
    void setToneStackEnabled(bool toneStackEnabled);

    void loadModelButtonClicked();

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

    knobLookAndFeel lnf;

    juce::String ngThreshold {"Null"};

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment;

    std::unique_ptr<juce::TextEditor> modelNameBox;
    std::unique_ptr<juce::TextButton> loadModelButton;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)
};
