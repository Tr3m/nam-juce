#pragma once
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
#include "AssetManager.h"
#include "PresetManager/PresetManagerComponent.h"
#include "juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h"
#include "WebComponent.h"

class TopBarComponent : public juce::AudioProcessorEditor,
                        public juce::ComboBox::Listener
{
public:
    TopBarComponent(NamJUCEAudioProcessor&, std::function<void()>&& updateFunction);
    ~TopBarComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setBackgroundColour(juce::Colour colour);

    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void openBrowserWindow();
    void openInfoWindow();

    enum DropdownOptions
    {
        AudioSettings = 0,
        GetModels,
        Info
    };
    
private:

    PresetManagerComponent pmc;
    std::function<void()> parentUpdater;

    std::unique_ptr<juce::ComboBox> settingsDropdown;
    std::unique_ptr<juce::ImageButton> settingsButton;

    juce::Image settingsPushed = juce::ImageFileFormat::loadFrom(BinaryData::settingspushed_png, BinaryData::settingspushed_pngSize);
    juce::Image settingsUnpushed = juce::ImageFileFormat::loadFrom(BinaryData::settingsunpushed_png, BinaryData::settingsunpushed_pngSize);

    juce::Colour backgroundColour {juce::Colours::transparentBlack};

    juce::LookAndFeel_V4 lnf;

    SafePointer<DialogWindow> dialogWindow;

    NamJUCEAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopBarComponent)
};