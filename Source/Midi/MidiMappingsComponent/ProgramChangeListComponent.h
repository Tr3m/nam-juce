#ifndef __PROGRAM_CHANGE_LIST_COPMPONENT_H__
#define __PROGRAM_CHANGE_LIST_COPMPONENT_H__

#include "ProgramChangeEntryComponent.h"
#include "../../PluginProcessor.h"

class ProgramChangeListComponent : public juce::Component
{
public:
    ProgramChangeListComponent(NamJUCEAudioProcessor& p)
        : audioProcessor(p), presetList(p.getAllPresets())
    {
        audioProcessor.getMidiHandler().loadPcConfig(mappings);

        for (int i = 0; i < 128; ++i) 
        {
            ProgramChangeEntryComponent* m = new ProgramChangeEntryComponent(i, this->presetList,
                    [&](int index, const juce::String& presetName){setNewMapping(index, presetName);}, mappings[i]);

            addAndMakeVisible(*m);
            entries.push_back(std::move(m));
        }
    };

    ~ProgramChangeListComponent()
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
        juce::Rectangle<int> r (0, 0, getWidth(), 6000);
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

    void setNewMapping(int index, const juce::String& preset)
    {
        if (index < mappings.size())
            mappings.set(index, preset);

        // Write to file
        DBG("Set Program " + juce::String(index) + " to preset " + preset);
        DBG(mappings.size());

        audioProcessor.getMidiHandler().savePcConfig(mappings);
    }

private:
    std::vector<ProgramChangeEntryComponent*> entries;
    juce::StringArray presetList;
    juce::StringArray mappings;

    NamJUCEAudioProcessor& audioProcessor;
};

#endif //__PROGRAM_CHANGE_LIST_COPMPONENT_H__
