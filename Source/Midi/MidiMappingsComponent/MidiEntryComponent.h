#ifndef __MIDI_ENTRY_COMPONENT_H__
#define __MIDI_ENTRY_COMPONENT_H__

#include <JuceHeader.h>
#include "../Mappings.h"

class MidiEntryComponent : public juce::Component,
                           public juce::ComboBox::Listener
{
public:
    MidiEntryComponent(const MidiMappingDisplay& mapping, juce::StringArray paramIDs, juce::StringArray paramNames)
        : paramNames(paramNames), paramIDs(paramIDs)
    {
        paramComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(paramComboBox.get());
        for (int i = 0; i < paramIDs.size(); ++i)
            paramComboBox->addItem(paramNames[i], i+1);
        
        paramComboBox->setSelectedId(paramNames.indexOf(mapping.parameterName) + 1, juce::NotificationType::dontSendNotification);


        valueComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(valueComboBox.get());
        for (int i = 1; i < 128; ++ i)
            valueComboBox->addItem("#" + juce::String(i), i);

        valueComboBox->setSelectedId(mapping.ccNumber, juce::NotificationType::dontSendNotification);

        channelComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(channelComboBox.get());
        channelComboBox->addItem("Omni", 1);
        for (int i = 2; i <= 17; ++i)
            channelComboBox->addItem(juce::String(i - 1), i);

        channelComboBox->setSelectedId(mapping.channel + 1, juce::NotificationType::dontSendNotification);

        deleteButton.reset(new juce::TextButton());
        addAndMakeVisible(deleteButton.get());
        deleteButton->setButtonText("X");
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
        int spacing = 20;
        // int width = r.proportionOfWidth(0.6) + getHeight() + (3 * spacing);
        int width = r.proportionOfWidth(0.6) + (2 * spacing);

        paramComboBox->setBounds(getWidth() / 2 - width / 2, 0, r.proportionOfWidth(0.3f) - spacing, getHeight());
        valueComboBox->setBounds(paramComboBox->getX() + paramComboBox->getWidth() + spacing, 0, r.proportionOfWidth(0.15f) - spacing, getHeight());
        channelComboBox->setBounds(valueComboBox->getX() + valueComboBox->getWidth() + spacing, 0, r.proportionOfWidth(0.15), getHeight());
        deleteButton->setBounds(channelComboBox->getX() + channelComboBox->getWidth() + spacing, 0, getHeight(), getHeight());
    };

    void comboBoxChanged(juce::ComboBox* cb) override
    {

    };

private:
    std::unique_ptr<juce::ComboBox> paramComboBox, valueComboBox, channelComboBox;
    std::unique_ptr<juce::TextButton> deleteButton;
    juce::StringArray paramIDs, paramNames;

};

#endif // __MIDI_ENTRY_COMPONENT_H__
