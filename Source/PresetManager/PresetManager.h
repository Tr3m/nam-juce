#pragma once
//#include <JuceHeader.h>
#include "../JuceLibraryCode/JuceHeader.h"


class PresetManager : juce::ValueTree::Listener
{
public:

    PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager();

    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;

    static const juce::File defaultPresetDirectory;
    static const juce::String presetExtension;
    static const juce::String presetNameProperty;

private:   
    void valueTreeRedirected(juce::ValueTree& treeChanged) override;

    juce::AudioProcessorValueTreeState& apvts;
    juce::Value currentPreset;

};