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
    void loadIrButtonClicked();

    enum PluginKnobs
    {
        Input = 0,
        NoiseGate,
        Bass,
        Middle,
        Treble,
        Output,
        LowCut,
        HighCut
    };

private:

    std::unique_ptr<juce::Slider> sliders[8];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[8];

    knobLookAndFeel lnf;

    juce::String ngThreshold {"Null"};

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle, irToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment, irToggleAttachment;

    std::unique_ptr<juce::TextEditor> modelNameBox, irNameBox;
    std::unique_ptr<juce::TextButton> loadModelButton, loadIRButton;

    foleys::LevelMeter meterIn{ foleys::LevelMeter::SingleChannel }, meterOut{ foleys::LevelMeter::SingleChannel };
    MeterLookAndFeel meterlnf;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)

// Private Functions
private:
    void initializeTextBox(const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height);
    void initializeButton(const juce::String label, std::unique_ptr<juce::TextButton>& button, int x, int y, int width, int height);
    void initializeFilters();
};
