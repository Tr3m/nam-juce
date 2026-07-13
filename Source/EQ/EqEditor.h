#pragma once

#include "PluginProcessor.h"
#include "../MyLookAndFeel.h"

class EqFadeComponent : public juce::Component
{
public:

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::transparentBlack);
        g.drawImageAt(eqFadeImage, 0, 38);
    }

    void resized() override
    {

    };

private:    
    juce::Image eqFadeImage = juce::ImageFileFormat::loadFrom(BinaryData::eq_fade_png, BinaryData::eq_fade_pngSize);
};

class EqEditor : public juce::AudioProcessorEditor
{
public:
    EqEditor(NamJUCEAudioProcessor&, bool drawFade = false);
    ~EqEditor();

    void paint (juce::Graphics&) override;
    void resized () override;

    void toggleEq ();

    void placeSliders ();
    void updateSliders ();

    enum FrequencyBands
    {
        _31 = 0,
        _62,
        _125,
        _250,
        _500,
        _1K,
        _2K,
        _4K,
        _8K,
        _16K
    };

    void updateGraphics ();

private:
    juce::Image backgroundOn = juce::ImageFileFormat::loadFrom(BinaryData::eq_background_on_png, BinaryData::eq_background_on_pngSize);
    juce::Image backgroundOff = juce::ImageFileFormat::loadFrom(BinaryData::eq_background_off_png, BinaryData::eq_background_off_pngSize);

    juce::Slider inGainSlider, outGainSlider;
    CustomSlider sliders[10];

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[10], inputGainAttachment, outputGainAttachment;

    juce::Label inLevelLabel, outLevelLabel;

    juce::ToggleButton bypass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassButtonAttachment;

    SliderLookAndFeel lnfOn {SliderLookAndFeel::Status::ON, SliderLookAndFeel::Orientation::Vertical};
    SliderLookAndFeel lnfOff {SliderLookAndFeel::Status::OFF, SliderLookAndFeel::Orientation::Vertical};

    knobLookAndFeel lnf{knobLookAndFeel::KnobTypes::Minimal};

    int globalOffset{16};

    std::unique_ptr<EqFadeComponent> fadeComponent;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqEditor)
};
