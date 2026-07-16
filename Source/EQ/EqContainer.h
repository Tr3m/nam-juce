#pragma once
#include "PluginProcessor.h"
#include "EqEditor.h"
#include "../MyLookAndFeel.h"

class EqContainer : public juce::AudioProcessorEditor,
                    private juce::Value::Listener
{
public:
    EqContainer(NamJUCEAudioProcessor&, std::unique_ptr<EqContainer>&);
    ~EqContainer();

    void paint (juce::Graphics&) override;
    void resized () override;
    void updateGraphics();


private:
    EqEditor eqEditor, dummyEditor;
    std::unique_ptr<EqContainer>& self;

    void valueChanged (Value& value ) override;
    
    juce::ImageButton closeButton;
    juce::Image xIcon = juce::ImageFileFormat::loadFrom(BinaryData::xIcon_png, BinaryData::xIcon_pngSize);

    foleys::LevelMeter meterIn{foleys::LevelMeter::SingleChannel}, meterOut{foleys::LevelMeter::SingleChannel};
    MeterLookAndFeel meterlnf;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqContainer)
};
