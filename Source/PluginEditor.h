#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "NamEditor.h"

//==============================================================================
/**
 */
class NamJUCEAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NamJUCEAudioProcessorEditor(NamJUCEAudioProcessor&);
    ~NamJUCEAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized () override;

    float calculateEditorScale(int currentWidth);

private:
    NamEditor namEditor;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamJUCEAudioProcessorEditor)
};
