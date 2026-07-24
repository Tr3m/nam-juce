#ifndef __MIDI_DIALOG_BOX_H__
#define __MIDI_DIALOG_BOX_H__

#include <JuceHeader.h>
#include "../../PluginProcessor.h"

class MidiDialogBox : public juce::Component, private juce::TextEditor::Listener
{
public:

    MidiDialogBox(NamJUCEAudioProcessor& p, std::function<void()>&& parentUpdater) 
        : audioPocessor(p), cancelOrOkClicked(std::move(parentUpdater))
    {
        // this->getLookAndFeel().setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromString("FF121212").withAlpha(0.7f));
        // this->getLookAndFeel().setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromString("FF121212").withAlpha(0.7f));
        lnf.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);

        addAndMakeVisible(&presetName);
        presetName.addListener(this);
        presetName.setJustification(juce::Justification::centred);
        presetName.setInputRestrictions(30, allowedCharacters);

        presetName.onReturnKey = [this] { savePreset(); };
        presetName.onEscapeKey = [this] { cancelOrOkClicked(); };

        addAndMakeVisible(&saveButton);
        saveButton.setButtonText("Save");
        saveButton.setLookAndFeel(&lnf);

        if (presetName.getText().trim().isEmpty())
            saveButton.setEnabled(false);

        saveButton.onClick = [this] { this->savePreset(); };

        addAndMakeVisible(&cancelButton);
        cancelButton.setButtonText("Cancel");
        cancelButton.setLookAndFeel(&lnf);

        cancelButton.onClick = [this]
        {
            presetName.clear();
            cancelOrOkClicked();
        };

        this->getLookAndFeel().setUsingNativeAlertWindows(true);
    };

    ~MidiDialogBox()
    {
    };

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colour::fromString("ff1e1e1e"));
        g.fillRoundedRectangle(titleArea.toFloat(), 8.0f);
        g.fillRect(titleArea.toFloat().withY(5));

        g.setFont(18.0f);
        g.setColour(juce::Colours::white);
        g.drawFittedText("Save Preset", titleArea.withY(3), juce::Justification::centred, 1);
    };

    void resized() override
    {
        int boxHeight = 25;
        presetName.setBounds(20, (getHeight() / 2) - boxHeight, getWidth() - 40, boxHeight);
        saveButton.setBounds(presetName.getX(), presetName.getY() + presetName.getHeight() + 20, (presetName.getWidth() / 2) - 3, presetName.getHeight());
        cancelButton.setBounds(saveButton.getX() + saveButton.getWidth() + 3, saveButton.getY(), saveButton.getWidth(), saveButton.getHeight());
        titleArea.setBounds(0, 0, getWidth(), 35);
    };

    void getKbFocus()
    {
        this->grabKeyboardFocus();
        presetName.grabKeyboardFocus();
    }

    void setInputFieldText(const juce::String& text)
    {
        presetName.setText(text);

        if (!text.isEmpty())
            presetName.selectAll();
    }

private: 
    juce::TextButton saveButton, cancelButton;
    juce::TextEditor presetName;
    juce::LookAndFeel_V4 lnf;
    juce::Rectangle<int> titleArea;
    const juce::String allowedCharacters {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 "};
    

    std::function<void()> cancelOrOkClicked;

    NamJUCEAudioProcessor& audioPocessor;

    void savePreset()
    {
        auto& mh = audioPocessor.getMidiHandler();
        mh.saveConfig(mh.midiDirectory.getChildFile(presetName.getText() + ".xml"));
        cancelOrOkClicked();
    }

    void textEditorTextChanged (TextEditor& editor) override
    {
        if (&editor == &presetName)
        {
            if (editor.getText().trim().isEmpty())
                saveButton.setEnabled(false);
            else
                saveButton.setEnabled(true);
        }
    }
};

#endif
