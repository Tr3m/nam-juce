#pragma once
// #include <JuceHeader.h>
#include "../JuceLibraryCode/JuceHeader.h"


class PresetManager : juce::ValueTree::Listener
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager();

    void savePreset (const juce::String& presetName);
    void loadPreset (const juce::String& presetName);
    void deletePreset (const juce::String& presetName);

    int getNextPresetIndex ();
    int getPreviousPresetIndex ();
    
    bool loadNextPreset();
    bool loadPreviousPreset();

    juce::StringArray getAllPresets () const;
    juce::String getCurrentPreset () const;

    static const juce::File defaultPresetDirectory;
    static const juce::String presetExtension;
    static const juce::String presetNameProperty;

private:
    void valueTreeRedirected (juce::ValueTree& treeChanged) override;

    juce::StringArray ignoredParams {"PRESET_NEXT_ID", "PRESET_PREVIOUS_ID", "MODEL_NEXT_ID", "MODEL_PREVIOUS_ID", "IR_NEXT_ID", "IR_PREVIOUS_ID"};

    juce::AudioProcessorValueTreeState& apvts;
    juce::Value currentPreset;
};
