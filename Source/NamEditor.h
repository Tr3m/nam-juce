#pragma once

#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
#include "AssetManager.h"
#include "EqEditor.h"
#include "TopBarComponent.h"
#include "LedButtonComponent.h"

#define NUM_SLIDERS 9

class NamEditor : public juce::AudioProcessorEditor, public juce::Timer, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:
    NamEditor(NamJUCEAudioProcessor&);
    ~NamEditor() override;

    void paint (juce::Graphics&) override;
    void resized () override;

    void timerCallback ();
    void sliderValueChanged (juce::Slider* slider) override;
    void comboBoxChanged (juce::ComboBox* comboBox) override;

    void setToneStackEnabled (bool toneStackEnabled);

    void setMeterPosition (bool isOnMainScreen);

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
    std::unique_ptr<juce::ComboBox> modelComboBox;

    juce::String sliderIDs[NUM_SLIDERS]{
        "INPUT_ID", "NGATE_ID", "BASS_ID", "MIDDLE_ID", "TREBLE_ID", "OUTPUT_ID", "LOWCUT_ID", "HIGHCUT_ID", "DOUBLER_ID"};

    std::unique_ptr<AssetManager> assetManager;

    // juce::TooltipWindow tooltipWindow{ this, 200 };

    knobLookAndFeel lnf{knobLookAndFeel::KnobTypes::Main};
    SliderLookAndFeel slimLnfOn {SliderLookAndFeel::Status::ON, SliderLookAndFeel::Orientation::Horizontal};
    SliderLookAndFeel slimLnfOff {SliderLookAndFeel::Status::OFF, SliderLookAndFeel::Orientation::Horizontal};

    juce::String ngThreshold{"Null"};

    int screensOffset = 46;

    std::unique_ptr<juce::ToggleButton> toneStackToggle, normalizeToggle, irToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneStackToggleAttachment, normalizeToggleAttachment, irToggleAttachment;

    std::unique_ptr<juce::ImageButton> loadModelButton, loadIRButton, clearIrButton, clearModelButton;
    std::unique_ptr<LedButtonComponent> toneStackButton, normalizeButton, irButton, eqButton;
    juce::Image xIcon = juce::ImageFileFormat::loadFrom(BinaryData::xIcon_png, BinaryData::xIcon_pngSize);

    //// TODO: Move this into a dedicated component with its own timer
    juce::Image led_off = juce::ImageFileFormat::loadFrom(BinaryData::led_off_png, BinaryData::led_off_pngSize);
    juce::Image led_on = juce::ImageFileFormat::loadFrom(BinaryData::led_on_png, BinaryData::led_on_pngSize);
    juce::Image led_to_draw;

    std::unique_ptr<juce::TextEditor> modelNameBox, irNameBox;

    foleys::LevelMeter meterIn{foleys::LevelMeter::SingleChannel}, meterOut{foleys::LevelMeter::SingleChannel};
    MeterLookAndFeel meterlnf, meterlnf2;

    EqEditor eqEditor;

    TopBarComponent topBar;

    NamJUCEAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamEditor)

    // Private Functions
private:
    void initializeTextBox (const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height);
    void initializeButton (const juce::String label, const juce::String buttonText, std::unique_ptr<juce::ImageButton>& button, int x, int y,
                           int width, int height);

    // Pass this to the Preset Manager for updating the gui after loading a new preset.
    // Maybe not the best way of doing it...
    void updateAfterPresetLoad ();

    void populateModelComboBox();
};
