#pragma once

#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
#include "AssetManager.h"
#include "EQ/EqContainer.h"
#include "TopBarComponent.h"
#include "LedButtonComponent.h"
#include "PresetManager/PresetDialogBoxWrapper.h"
#include "Midi/MidiMappingsComponent/MidiMappingsComponentWrapper.h"
#include "LedComponent.h"

#define NUM_SLIDERS 9

class NamEditor : public juce::AudioProcessorEditor,
                  juce::Slider::Listener, 
                  juce::ComboBox::Listener,
                  juce::Value::Listener
{
public:
    NamEditor(NamJUCEAudioProcessor&);
    ~NamEditor() override;

    void paint (juce::Graphics&) override;
    void resized () override;

    void sliderValueChanged (juce::Slider* slider) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;
    void valueChanged (Value& value ) override;

    void setToneStackEnabled (bool toneStackEnabled);

    void loadModelButtonClicked ();
    void loadIrButtonClicked ();

    enum PluginKnobs
    {
        Input = 0,
        NoiseGate,
        Bass,
        Middle,
        Treble,
        Output,
        LowCut,
        HighCut,
        Doubler
    };


private:
    std::unique_ptr<CustomSlider> sliders[NUM_SLIDERS];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[NUM_SLIDERS];
    std::unique_ptr<CustomSlider> slimSlider;
    std::unique_ptr<juce::ComboBox> modelComboBox, irComboBox;

    juce::String sliderIDs[NUM_SLIDERS]{"INPUT_ID", "NGATE_ID", "BASS_ID", "MIDDLE_ID", "TREBLE_ID", "OUTPUT_ID", "LOWCUT_ID", "HIGHCUT_ID", "DOUBLER_ID"};

    std::unique_ptr<AssetManager> assetManager;

    knobLookAndFeel lnf{knobLookAndFeel::KnobTypes::Main};
    SliderLookAndFeel slimLnfOn {SliderLookAndFeel::Status::ON, SliderLookAndFeel::Orientation::Horizontal};
    SliderLookAndFeel slimLnfOff {SliderLookAndFeel::Status::OFF, SliderLookAndFeel::Orientation::Horizontal};

    int screensOffset = 46;

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle, irToggle, eqToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment, irToggleAttachment, eqToggleAttachment;

    std::unique_ptr<juce::ImageButton> loadModelButton, loadIRButton, clearIrButton, clearModelButton, prevModelButton, nextModelButton, prevIrButton, nextIrButton;
    std::unique_ptr<LedButtonComponent> toneStackButton, normalizeButton, irButton, eqButton;

    std::unique_ptr<LedComponent> ledComponent;

    std::unique_ptr<juce::TextEditor> modelNameBox, irNameBox;

    foleys::LevelMeter meterIn{foleys::LevelMeter::SingleChannel}, meterOut{foleys::LevelMeter::SingleChannel};
    MeterLookAndFeel meterlnf;

    std::unique_ptr<EqContainer> eqEditor;

    TopBarComponent topBar;

    std::unique_ptr<PresetDialogBoxWrapper> presetDialog;
    void showSaveDialog(const juce::String&);

    std::unique_ptr<MidiMappingsComponentWrapper> mappingsComp;
    void showMappingsComponent();

    juce::TooltipWindow tooltipWindow{this, 300};

    NamJUCEAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamEditor)

private:
    void initializeTextBox (const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height);
    void initializeButton (const juce::String label, const juce::String buttonText, std::unique_ptr<juce::ImageButton>& button, int x, int y, int width, int height);

    // Pass this to the Preset Manager for updating the gui after loading a new preset.
    // Maybe not the best way of doing it...
    void updateAfterPresetLoad (bool isFromMidi = false);

    void updateModelBox();
    void updateIrBox();

    void populateModelComboBox();
    void populateIrComboBox();

    void showEqModule();
};
