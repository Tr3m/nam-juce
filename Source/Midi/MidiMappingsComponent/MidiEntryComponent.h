#ifndef __MIDI_ENTRY_COMPONENT_H__
#define __MIDI_ENTRY_COMPONENT_H__

#include <JuceHeader.h>
#include "../Mappings.h"
#include "../../LookAndFeel/LookAndFeel.h"

class MidiEntryComponent : public juce::Component,
                           public juce::ComboBox::Listener
{
public:
    MidiEntryComponent(MidiMappingDisplay midiMapping, juce::StringArray paramIDs, juce::StringArray paramNames,
            std::function<void(uint32_t)>&& deleteEntry,
            std::function<void(uint32_t, int)>&& changeCC, 
            std::function<void(uint32_t, int)>&& changeChannel,
            std::function<void(uint32_t id, const juce::String& parameterID)>&& changeParameter)
        : paramNames(paramNames), paramIDs(paramIDs), 
            mapping(midiMapping), deletePressed(std::move(deleteEntry)),
            ccChanged(std::move(changeCC)), channelChanged(std::move(changeChannel)),
            paramIdChanged(std::move(changeParameter))
    {

        cbLNF.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        cbLNF.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        cbLNF.setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        cbLNF.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        cbLNF.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, juce::Colour::fromString("FF2E2E2E").withAlpha(1.0f));

        paramComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(paramComboBox.get());
        for (int i = 0; i < paramIDs.size(); ++i)
            paramComboBox->addItem(paramNames[i], i+1);
        
        paramComboBox->setSelectedId(paramNames.indexOf(mapping.parameterName) + 1, juce::NotificationType::dontSendNotification);
        paramComboBox->addListener(this);
        paramComboBox->setLookAndFeel(&cbLNF);

        valueComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(valueComboBox.get());
        for (int i = 0; i < 128; ++ i)
            valueComboBox->addItem("#" + juce::String(i), i+1);

        valueComboBox->setSelectedId(mapping.ccNumber + 1, juce::NotificationType::dontSendNotification);
        valueComboBox->addListener(this);
        valueComboBox->setLookAndFeel(&cbLNF);

        channelComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(channelComboBox.get());
        channelComboBox->addItem("Omni", 1);
        for (int i = 2; i <= 17; ++i)
            channelComboBox->addItem(juce::String(i - 1), i);

        channelComboBox->setSelectedId(mapping.channel + 1, juce::NotificationType::dontSendNotification);
        channelComboBox->addListener(this);
        channelComboBox->setLookAndFeel(&cbLNF);

        deleteButton.reset(new juce::TextButton());
        addAndMakeVisible(deleteButton.get());
        // deleteButton->setButtonText("X");
        deleteButton->setLookAndFeel(&lnf);
        deleteButton->onClick = [this]
        { 
            DBG("Deleting mapping for " + mapping.parameterName);
            deletePressed(mapping.uid); 
        };
    };


    ~MidiEntryComponent()
    {
        valueComboBox->removeListener(this);
        valueComboBox.reset();
        paramComboBox->removeListener(this);
        paramComboBox.reset();
        channelComboBox->removeListener(this);
        channelComboBox.reset();
    };

    void paint(juce::Graphics& g) override
    {
        // g.fillAll(juce::Colours::green);
    };

    void resized() override
    {
        juce::Rectangle<int> r (getWidth(), getHeight());
        int spacing = 10;
        int width = r.proportionOfWidth(0.6) + (2 * spacing) + getHeight();

        paramComboBox->setBounds(getWidth() / 2 - width / 2, 0, r.proportionOfWidth(0.3f) - spacing, getHeight());
        valueComboBox->setBounds(paramComboBox->getX() + paramComboBox->getWidth() + spacing, 0, r.proportionOfWidth(0.15f) - spacing, getHeight());
        channelComboBox->setBounds(valueComboBox->getX() + valueComboBox->getWidth() + spacing, 0, r.proportionOfWidth(0.15), getHeight());
        deleteButton->setBounds(channelComboBox->getX() + channelComboBox->getWidth() + spacing, 0, getHeight(), getHeight());
    };

    void comboBoxChanged(juce::ComboBox* cb) override
    {
        if (cb == paramComboBox.get())
        {
            DBG("Chaning paramterer" + juce::String(mapping.uid) + " from " + mapping.parameterName + " to "
                    + paramIDs[paramComboBox->getSelectedId() - 1]);

            paramIdChanged(mapping.uid, paramIDs[paramComboBox->getSelectedId() - 1]);
        }
        else if (cb == valueComboBox.get())
        {
            DBG("Changing CC of " + mapping.parameterID + " (" + juce::String(mapping.uid) + ") to "
                    + juce::String(valueComboBox->getSelectedId() - 1));

            ccChanged(mapping.uid, valueComboBox->getSelectedId() - 1);
        }
        else if (cb == channelComboBox.get())
        {
            DBG("Changing Channel of " + mapping.parameterID + " (" + juce::String(mapping.uid) + ") to "
                    + juce::String(channelComboBox->getSelectedId() - 1));

            channelChanged(mapping.uid, channelComboBox->getSelectedId() - 1);
        }
    };

private:
    MidiMappingDisplay mapping;
    std::unique_ptr<juce::ComboBox> paramComboBox, valueComboBox, channelComboBox;
    std::unique_ptr<juce::TextButton> deleteButton;
    juce::StringArray paramIDs, paramNames;
    SimpleQuitButtonLNF lnf;
    ComboBoxLNF cbLNF;
    
    // Parent Callbacks
    std::function<void(uint32_t)> deletePressed;
    std::function<void(uint32_t, int)> ccChanged;
    std::function<void(uint32_t, int)> channelChanged;
    std::function<void(uint32_t id, const juce::String& parameterID)> paramIdChanged;

};

#endif // __MIDI_ENTRY_COMPONENT_H__
