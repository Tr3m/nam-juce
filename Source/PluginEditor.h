#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

#define NUM_SLIDERS 8

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

    std::unique_ptr<juce::Slider> sliders[NUM_SLIDERS];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[NUM_SLIDERS];

    juce::String sliderIDs[NUM_SLIDERS] {"INPUT_ID", "NGATE_ID", "BASS_ID", "MIDDLE_ID", "TREBLE_ID", "OUTPUT_ID", "LOWCUT_ID", "HIGHCUT_ID"};

    knobLookAndFeel lnf;

    juce::String ngThreshold {"Null"};

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle, irToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment, irToggleAttachment;

    std::unique_ptr<juce::TextEditor> modelNameBox, irNameBox;
    std::unique_ptr<juce::TextButton> loadModelButton, loadIRButton, clearIrButton, clearModelButton;

    foleys::LevelMeter meterIn{ foleys::LevelMeter::SingleChannel }, meterOut{ foleys::LevelMeter::SingleChannel };
    MeterLookAndFeel meterlnf;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)

// Private Functions
private:
    void initializeTextBox(const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height);
    void initializeButton(const juce::String label, const juce::String buttonText, std::unique_ptr<juce::TextButton>& button, int x, int y, int width, int height);
    void initializeFilters();
};
