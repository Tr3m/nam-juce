#pragma once
//#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"

class EqEditor : public juce::AudioProcessorEditor
{
public:

    EqEditor(NamJUCEAudioProcessor&);
    ~EqEditor();

    void paint(juce::Graphics&) override;
    void resized() override;

    void placeSliders();
    void updateSliders();

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

    void updateGraphics();

private:

    juce::Image backgroundOn = juce::ImageFileFormat::loadFrom(BinaryData::eq_background_on_png, BinaryData::eq_background_on_pngSize);
    juce::Image backgroundOff = juce::ImageFileFormat::loadFrom(BinaryData::eq_background_off_png, BinaryData::eq_background_off_pngSize);

    juce::Slider sliders[10], inGainSlider, outGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments[10], inputGainAttachment, outputGainAttachment;

    juce::Label inLevelLabel, outLevelLabel;

    juce::ToggleButton bypass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassButtonAttachment;

    SliderOnLookAndFeel lnfOn;
    SliderOffLookAndFeel lnfOff;

    knobLookAndFeel lnf {knobLookAndFeel::KnobTypes::Minimal};
    
    int globalOffset {23};

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqEditor)

};