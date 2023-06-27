#pragma once

#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
#include "AssetManager.h"
#include "EqEditor.h"
#include "PresetManager/PresetManagerComponent.h"

#define NUM_SLIDERS 8

class NamEditor : public juce::AudioProcessorEditor, public juce::Timer, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:

    NamEditor(NamJUCEAudioProcessor&);
    ~NamEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback();
    void sliderValueChanged(juce::Slider* slider);

    void setToneStackEnabled(bool toneStackEnabled);

    void setMeterPosition(bool isOnMainScreen);

    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;


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


    std::unique_ptr<AssetManager> assetManager;   
        

    knobLookAndFeel lnf {knobLookAndFeel::KnobTypes::Main};

    juce::String ngThreshold {"Null"};

    int screensOffset = 46;

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle, irToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment, irToggleAttachment;

    std::unique_ptr<juce::ImageButton> toneStackButton, normalizeButton, irButton, loadModelButton, loadIRButton, clearIrButton, clearModelButton, eqButton;
    juce::Image xIcon = juce::ImageFileFormat::loadFrom(BinaryData::xIcon_png, BinaryData::xIcon_pngSize);

    //// TODO: Move this into a dedicated component with its own timer
    juce::Image led_off = juce::ImageFileFormat::loadFrom(BinaryData::led_off_png, BinaryData::led_off_pngSize);
    juce::Image led_on = juce::ImageFileFormat::loadFrom(BinaryData::led_on_png, BinaryData::led_on_pngSize);
    juce::Image led_to_draw;

    std::unique_ptr<juce::TextEditor> modelNameBox, irNameBox;


    foleys::LevelMeter meterIn{ foleys::LevelMeter::SingleChannel }, meterOut{ foleys::LevelMeter::SingleChannel };
    MeterLookAndFeel meterlnf, meterlnf2;

    EqEditor eqEditor;

    PresetManagerComponent pmc;
    
    NamJUCEAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamEditor)

// Private Functions
private:
    void initializeTextBox(const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height);
    void initializeButton(const juce::String label, const juce::String buttonText, std::unique_ptr<juce::ImageButton>& button, int x, int y, int width, int height);
};