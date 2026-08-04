#ifndef __PRESET_DIALOG_BOX_WRAPPER_H__
#define __PRESET_DIALOG_BOX_WRAPPER_H__

#include <JuceHeader.h>
#include "PresetDialogBox.h"
#include "PresetManager.h"
#include "PresetManagerComponent.h"

class PresetDialogBoxWrapper : public juce::Component, private juce::MouseListener
{
public:

    PresetDialogBoxWrapper(PresetManager& pm, PresetManagerComponent* pmc, std::unique_ptr<PresetDialogBoxWrapper>& selfRef)
        : self(selfRef)
    {
        dBox.reset(new PresetDialogBox(pm, pmc, [&]() {destroy();}));
        addAndMakeVisible(dBox.get());
    };

    ~PresetDialogBoxWrapper()
    {
        dBox = nullptr;
        DBG("Goodbye Preset Dialog Box! (Destructor)");
    };

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.6f));

        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRoundedRectangle(componentBounds.toFloat(), 8.0f);
    };

    void resized() override
    {
        int boxWidth = getWidth() / 4;
        int boxHeight = getHeight() / 4;
        componentBounds.setBounds(getWidth() / 2 - boxWidth / 2, getHeight() / 2 - boxHeight / 2, boxWidth, boxHeight);
        dBox->setBounds(componentBounds);
        this->repaint();
    };

    void destroy()
    {
       removeChildComponent(dBox.get());
       dBox = nullptr;

       DBG("Goodbye Preset Dialog Box!");
       self.reset();
    }

    void setInputFieldText(const juce::String& text)
    {
        dBox->setInputFieldText(text);        
    }

    void setPresetListComponent(juce::ComboBox* comboBox, int* currentSelection)
    {
        dBox->setPresetListComponent(comboBox, currentSelection);
    }

private:
    std::unique_ptr<PresetDialogBox> dBox;
    std::unique_ptr<PresetDialogBoxWrapper>& self;
    juce::Rectangle<int> componentBounds;

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.mouseWasClicked())
            destroy();
    };
};

#endif
