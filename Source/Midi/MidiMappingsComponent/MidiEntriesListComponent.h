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
        reconstructUI(false);
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

    void addDefaultEntry()
    {
        DBG("Adding default mapping");
        audioProcessor.getMidiHandler().addMapping(audioProcessor.getParameterIDs()[0], 1, 0, audioProcessor.apvts);
        reconstructUI();
    }

    void deleteEntry(uint32_t id)
    {
        audioProcessor.getMidiHandler().removeMapping(id);
        reconstructUI();
    };

    void setEntryCC(uint32_t id, int cc)
    {
        audioProcessor.getMidiHandler().setMappingCC(id, cc);
    }

    void setEntryChannel(uint32_t id, int channel)
    {
        audioProcessor.getMidiHandler().setMappingChannel(id, channel);
    }

    void setEntryParameter(uint32_t id, const juce::String& parameterID)
    {
        audioProcessor.getMidiHandler().setMappingParameter(id, parameterID, audioProcessor.apvts);
    }

    void reconstructUI(bool deleteEntries = true)
    {
        if (deleteEntries)
        {
            for (auto* entry : entries)
                delete entry;

            entries.clear();

            this->removeChildComponent(&addButton);
        }

        auto mappings = audioProcessor.getMidiHandler().getMappingsForDisplay();

        addAndMakeVisible(&addButton);
        addButton.setButtonText("+");
        addButton.onClick = [this] { addDefaultEntry(); };

        for (auto& mapping : mappings)
        {

            MidiEntryComponent* m = new MidiEntryComponent(mapping, audioProcessor.getParameterIDs(), audioProcessor.getParameterNames(), 
                    [&](uint32_t id){ deleteEntry(id); }, [&](uint32_t id, int cc){ setEntryCC(id, cc);}, 
                    [&](uint32_t id, int channel){ setEntryChannel(id, channel);},
                    [&](uint32_t id, const juce::String& parameterID){ setEntryParameter(id, parameterID); });

            addAndMakeVisible(*m);
            entries.push_back(std::move(m));
        }

        this->resized();
        this->updateSize(this->getWidth());
    }

private:
    std::vector<MidiEntryComponent*> entries;
    juce::TextButton addButton;
    NamJUCEAudioProcessor& audioProcessor;
};

#endif // __MIDI_ENTRIES_LIST_COMPONENT_H__
