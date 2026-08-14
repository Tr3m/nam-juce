#pragma once
#include "PluginProcessor.h"
#include "LookAndFeel/LookAndFeel.h"
#include "AssetManager.h"
#include "PresetManager/PresetManagerComponent.h"
#include "Preferences/PreferencesWindow.h"
#include "InfoComponent.h"
#include <CustomStandaloneWindow/CustomStandaloneFilterWindow.h>

class TopBarComponent : public juce::AudioProcessorEditor, public juce::ComboBox::Listener
{
public:
    TopBarComponent(NamJUCEAudioProcessor&, std::function<void()>&& updateFunction, 
                    std::function<void(juce::String)>&& showSavePresetDialogFunction,
                    std::function<void()>&& showMidiMappingsFunction,
                    std::function<void()>&& updateColourScheme);

    ~TopBarComponent() override;

    void paint (juce::Graphics& g) override;
    void resized () override;

    void setBackgroundColour (juce::Colour colour);

    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    enum DropdownOptions
    {
        AudioSettings = 0,
        Preferences,
        MidiMappings,
        GetModels,
        Info
    };

    PresetManagerComponent* getPresetManagerComponent() { return &this->pmc; };

    juce::PopupMenu* getSettingsRootMenu() { return settingsDropdown != nullptr ? settingsDropdown->getRootMenu() : nullptr; };
    juce::LookAndFeel* getMenuLookAndFeel() { return settingsDropdown != nullptr ? &settingsDropdown->getLookAndFeel() : nullptr; };
    void setMenuSelectedId (int);

private:
    PresetManagerComponent pmc;

    std::unique_ptr<juce::ComboBox> settingsDropdown;
    std::unique_ptr<juce::ImageButton> settingsButton;

    juce::Image settingsPushed = juce::ImageFileFormat::loadFrom(BinaryData::settingspushed_png, BinaryData::settingspushed_pngSize);
    juce::Image settingsUnpushed = juce::ImageFileFormat::loadFrom(BinaryData::settingsunpushed_png, BinaryData::settingsunpushed_pngSize);

    juce::Colour backgroundColour{juce::Colours::transparentBlack};

    ComboBoxLNF lnf;

    juce::URL modelsURL{"https://www.tone3000.com/"};
    
    void showInfoWindow();

    std::function<void()> showMidiMappings;

    void showPreferencesWindow();
    std::function<void()> colourSchemeChanged;

    NamJUCEAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopBarComponent)
};
