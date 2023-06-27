#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "NamEditor.h"
#include "PresetManager/PresetManagerComponent.h"

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

    void setPluginSize(bool makeSmall);

private:
    
    NamEditor namEditor;

    std::unique_ptr<juce::ImageButton> resizeButton;
    std::unique_ptr<juce::ToggleButton> hiddenResizeToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> resizeToggleAttachment;
    
    juce::Image arrowExpand = juce::ImageFileFormat::loadFrom(BinaryData::arrowexpand_png, BinaryData::arrowexpand_pngSize); 
    juce::Image arrowContract = juce::ImageFileFormat::loadFrom(BinaryData::arrowcontract_png, BinaryData::arrowcontract_pngSize);

    PresetManagerComponent pmc;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessorEditor)
};
