#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PresetManager.h"

class PresetManagerComponent : public juce::Component,
                               public juce::ComboBox::Listener
{
public:

    PresetManagerComponent(PresetManager&);

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void loadComboBox();
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void parameterChanged();

    juce::ComboBox* getComboBox() {return &presetComboBox;};

    void setColour(juce::Colour colourToUse, float alpha);
    void setColour(juce::Colour colourToUse);

private:
    int barHeight {32};
    juce::Colour barColour {juce::Colours::whitesmoke};
    float barAlpha {0.4f};

    juce::LookAndFeel_V4 lnf;

    juce::TextButton testButton;
    juce::TextEditor presetName;
    juce::ComboBox presetComboBox;

    juce::ImageButton saveButton, previousButton, nextButton;

    juce::Image savePushed = juce::ImageFileFormat::loadFrom(BinaryData::savepushed_png, BinaryData::savepushed_pngSize);
    juce::Image saveUnpushed = juce::ImageFileFormat::loadFrom(BinaryData::saveunpushed_png, BinaryData::saveunpushed_pngSize);
    juce::Image backPushed = juce::ImageFileFormat::loadFrom(BinaryData::backpushed_png, BinaryData::backpushed_pngSize);
    juce::Image backUnushed = juce::ImageFileFormat::loadFrom(BinaryData::backunpushed_png, BinaryData::backunpushed_pngSize);
    juce::Image forwardPushed = juce::ImageFileFormat::loadFrom(BinaryData::forwardpushed_png, BinaryData::forwardpushed_pngSize);
    juce::Image forwardUnpushed = juce::ImageFileFormat::loadFrom(BinaryData::forwardunpushed_png, BinaryData::forwardunpushed_pngSize);

    PresetManager& presetManager;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::TooltipWindow tooltipWindow { this, 300 };

    void constructUI();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManagerComponent)

};
