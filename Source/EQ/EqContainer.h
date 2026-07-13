#pragma once
#include "PluginProcessor.h"
#include "EqEditor.h"
#include "../MyLookAndFeel.h"

class EqContainer : public juce::AudioProcessorEditor,
                    private juce::Value::Listener
{
public:
    EqContainer(NamJUCEAudioProcessor&);
    ~EqContainer();

    void paint (juce::Graphics&) override;
    void resized () override;
    void toggleEq();
    void updateGraphics();


private:
    EqEditor eqEditor, dummyEditor;

    void valueChanged (Value& value ) override;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqContainer)
};
