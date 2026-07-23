#ifndef __MIDI_ENTRIES_LIST_COMPONENT_H__
#define __MIDI_ENTRIES_LIST_COMPONENT_H__

#include <JuceHeader.h>
#include "MidiEntryComponent.h"
#include "../../PluginProcessor.h"



class MidiEntriesListComponent : public juce::Component
{
public:
    MidiEntriesListComponent(NamJUCEAudioProcessor& p)
        : audioProcessor(p)
    {
        auto mappings = audioProcessor.getMidiHandler().getMappingsForDisplay();

        addAndMakeVisible(&addButton);
        addButton.setButtonText("+");
        
        for (const auto& mapping : mappings)
        {
            MidiEntryComponent* m = new MidiEntryComponent(mapping);
            addAndMakeVisible(*m);
            entries.push_back(std::move(m));
        }
    };

    ~MidiEntriesListComponent()
    {
        for (auto* entry : entries)
            delete entry;

        entries.clear();
    };

    void paint(juce::Graphics& g) override
    {
    };

    void resized() override
    {
        juce::Rectangle<int> r (0, 0, getWidth(), 3000);
        int itemSpacing = 5;
        int entryHeight = 30;

        for (const auto& entry : entries)
        {
            if (entry != nullptr)
            {
                entry->setBounds(r.removeFromTop(entryHeight));
                r.removeFromTop(itemSpacing);
            }
        }

        addButton.setBounds(r.removeFromTop(2 * entryHeight).withWidth(2 * entryHeight).withX(getWidth() / 2 - entryHeight / 2));

    };

    int getLowestY() const
    {
        int y = 0;

        for (auto* c : getChildren())
            y = jmax (y, c->getBottom());

        return y;
    }

    void updateSize(int parentWidth) 
    {
        this->setSize(parentWidth, this->getLowestY());
    }

private:
    std::vector<MidiEntryComponent*> entries;
    juce::TextButton addButton;
    NamJUCEAudioProcessor& audioProcessor;
};

#endif // __MIDI_ENTRIES_LIST_COMPONENT_H__
