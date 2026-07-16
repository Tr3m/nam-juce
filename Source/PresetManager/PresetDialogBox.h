#ifndef __PRESET_DIALOG_BOX_H__
#define __PRESET_DIALOG_BOX_H__

#include <JuceHeader.h>
#include "PresetManager.h"
#include "PresetManagerComponent.h"

class PresetDialogBox : public juce::Component, private juce::TextEditor::Listener
{
public:

    PresetDialogBox(PresetManager& pm, PresetManagerComponent* pmc, std::function<void()>&& parentUpdater) 
        : presetManager(pm), updateParent(std::move(parentUpdater))
    {
        this->getLookAndFeel().setColour(juce::TextEditor::backgroundColourId, juce::Colour::fromString("FF121212").withAlpha(0.7f));
        this->getLookAndFeel().setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromString("FF121212").withAlpha(0.7f));

        this->setPresetListComponent(pmc->getComboBox(), pmc->getCurrentSelection());

        addAndMakeVisible(&presetName);
        presetName.addListener(this);
        presetName.setJustification(juce::Justification::centred);
        presetName.setInputRestrictions(30, allowedCharacters);

        presetName.onReturnKey = [this] { savePreset(); };
        presetName.onEscapeKey = [this] { updateParent(); };

        addAndMakeVisible(&saveButton);
        saveButton.setButtonText("Save");

        if (presetName.getText().trim().isEmpty())
            saveButton.setEnabled(false);

        saveButton.onClick = [this] { this->savePreset(); };

        addAndMakeVisible(&cancelButton);
        cancelButton.setButtonText("Cancel");

        cancelButton.onClick = [this]
        {
            presetName.clear();
            updateParent();
        };

        this->getLookAndFeel().setUsingNativeAlertWindows(true);
    };

    ~PresetDialogBox()
    {
        presetList = nullptr;
        currentSelection = nullptr;
    };

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromString("FF121212").withAlpha(0.9f));
        g.setFont(18.0f);
        g.setColour(juce::Colours::white);
        g.drawFittedText("Save Preset", 0, 10, getWidth(), 35, juce::Justification::centred, 1);
    };

    void resized() override
    {

        int boxHeight = 25;
        presetName.setBounds(20, (getHeight() / 2) - boxHeight, getWidth() - 40, boxHeight);
        saveButton.setBounds(presetName.getX(), presetName.getY() + presetName.getHeight() + 10, (presetName.getWidth() / 2) - 3, presetName.getHeight());
        cancelButton.setBounds(saveButton.getX() + saveButton.getWidth() + 3, saveButton.getY(), saveButton.getWidth(), saveButton.getHeight());
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

    void setPresetListComponent(juce::ComboBox* comboBox, int* currentPresetId)
    {
        presetList = comboBox;
        currentSelection = currentPresetId;
    }

private: 
    PresetManager& presetManager;
    juce::TextButton saveButton, cancelButton;
    juce::TextEditor presetName;
    std::function<void()> updateParent;
    juce::ComboBox* presetList {nullptr};
    int* currentSelection {nullptr};
    juce::LookAndFeel_V4 lnf;
    const juce::String allowedCharacters {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@$%^&()[]+-_= "};

    void savePreset()
    {
        if (!presetName.getText().trim().isEmpty())
        {
            if (presetList != nullptr)
            {
                DBG("Saving Preset: " + presetName.getText().trim());
                presetManager.savePreset(presetName.getText().trim());

                updatePresetList();
                presetList = nullptr;

                presetName.clear();
                updateParent();
            }
        }
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

    void updatePresetList()
    {
        if (presetList != nullptr)
        {
            presetList->clear(juce::dontSendNotification);
    
            const auto allPresets = presetManager.getAllPresets();
            const auto currentPreset = presetManager.getCurrentPreset();
            presetList->addItemList(presetManager.getAllPresets(), 1);
            presetList->setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
            *currentSelection = presetList->getSelectedId();
        }
    }

};

#endif
