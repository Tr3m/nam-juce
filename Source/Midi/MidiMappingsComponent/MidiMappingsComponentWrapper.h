#ifndef __MIDI_MAPPINGS_COMPONENT_WRAPPER_H__
#define __MIDI_MAPPINGS_COMPONENT_WRAPPER_H__

#include "MidiMappingsComponent.h"

class MidiMappingsComponentWrapper : public juce::Component,
                                     public juce::MouseListener
{
public:
    
    MidiMappingsComponentWrapper(NamJUCEAudioProcessor& p, std::unique_ptr<MidiMappingsComponentWrapper>& selfRef)
        : self(selfRef)
    {
        lnf.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        lnf.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        lnf.setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        lnf.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));

        mappingsComp.reset(new MidiMappingsComponent(p));
        mappingsComp->setLookAndFeel(&lnf);
        addAndMakeVisible(mappingsComp.get());
        
        closeButton.reset(new juce::TextButton("X"));
        addAndMakeVisible(closeButton.get());
        closeButton->setLookAndFeel(&lnf);
        closeButton->onClick = [this] { this->destroy(); };
    }
    
    ~MidiMappingsComponentWrapper()
    {
        mappingsComp = nullptr;
        DBG("Goodbye MIDI Mappings (Dectructor)...");
    };

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.7f));

        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRoundedRectangle(componentArea.toFloat(), 8.0f);
    };

    void resized() override
    {
        int width = int(0.8 * getWidth());
        int height = int(0.8 * getHeight());

        componentArea.setBounds(getWidth() / 2 - width / 2, getHeight() / 2 - height / 2, width, height);

        mappingsComp->setBounds(componentArea);

        int buttonSize = 30;
        closeButton->setBounds(componentArea.getWidth() + componentArea.getX() - buttonSize - 5, componentArea.getY() + 7, buttonSize, buttonSize);
    };

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.mouseWasClicked())
            this->destroy();
    };

private:
    juce::Rectangle<int> componentArea;
    std::unique_ptr<MidiMappingsComponent> mappingsComp;
    std::unique_ptr<MidiMappingsComponentWrapper>& self;
    std::unique_ptr<juce::TextButton> closeButton;

    SimpleTextButtonLNF lnf;

    void destroy()
    {
        mappingsComp = nullptr;
        self.reset();

        DBG("Goodbye MIDI Mappings...");
    };
};

#endif //__MIDI_MAPPINGS_COMPONENT_WRAPPER_H__
