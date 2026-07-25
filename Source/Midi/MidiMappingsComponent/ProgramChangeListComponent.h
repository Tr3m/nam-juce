#ifndef __PROGRAM_CHANGE_LIST_COPMPONENT_H__
#define __PROGRAM_CHANGE_LIST_COPMPONENT_H__

#include "ProgramChangeEntryComponent.h"

class ProgramChangeListComponent : public juce::Component
{
public:
    ProgramChangeListComponent(const juce::StringArray& presetList)
        : presetList(presetList)
    {
        for (int i = 1; i <= 128; ++i) 
        {
            ProgramChangeEntryComponent* m = new ProgramChangeEntryComponent(i, this->presetList);
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

private:
    std::vector<ProgramChangeEntryComponent*> entries;
    juce::StringArray presetList;
};

#endif //__PROGRAM_CHANGE_LIST_COPMPONENT_H__
