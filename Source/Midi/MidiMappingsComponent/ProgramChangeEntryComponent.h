#ifndef __PROGRAM_CHANGE_ENTRY_COMPONENT_H__
#define __PROGRAM_CHANGE_ENTRY_COMPONENT_H__

#include <JuceHeader.h>

class ProgramChangeEntryComponent : public juce::Component,
                                    public juce::ComboBox::Listener
{

public:
    ProgramChangeEntryComponent(int programNumber, const juce::StringArray& presetList)
        : presetList(presetList)
    {
        this->programNumber = programNumber;

        presetSelect.reset(new juce::ComboBox());
        addAndMakeVisible(presetSelect.get());
        presetSelect->addItemList(this->presetList, 1);
        presetSelect->addListener(this); 
    };

    ~ProgramChangeEntryComponent()
    {

    };

    void paint(juce::Graphics& g) override
    {
        // g.fillAll(juce::Colours::green);

        g.setColour(juce::Colours::black.withAlpha(0.45f));
        g.fillRoundedRectangle(labelArea.toFloat(), 8.0f);
        g.setColour(juce::Colours::snow);
        g.drawFittedText(juce::String(this->programNumber), labelArea, juce::Justification::centred, 1);
    };

    void resized() override
    {
        juce::Rectangle<int> r (getWidth(), getHeight());
        int spacing = 10;
        int width = r.proportionOfWidth(0.5) + spacing;
        int labelWidth = proportionOfWidth(0.1);

        presetSelect->setBounds(getWidth() / 2 - width / 2 + labelWidth + (spacing/2), 0, proportionOfWidth(0.4) - spacing, getHeight());
        labelArea.setBounds(presetSelect->getX() - labelWidth - (spacing / 2), presetSelect->getY(), labelWidth, getHeight());

    };

    void comboBoxChanged(juce::ComboBox* cb)
    {

    };

private:
    int programNumber {1};
    juce::Rectangle<int> labelArea;
    std::unique_ptr<juce::ComboBox> presetSelect;
    juce::StringArray presetList;
};

#endif //__PROGRAM_CHANGE_ENTRY_COMPONENT_H__
