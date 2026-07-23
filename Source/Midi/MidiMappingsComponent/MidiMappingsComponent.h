#ifndef __MIDI_MAPPINGS_COMPONENT_H__
#define __MIDI_MAPPINGS_COMPONENT_H__

#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "MidiEntriesListComponent.h"
#include "../../MyLookAndFeel.h"

class MidiMappingsComponent : public juce::Component
{
public:
    MidiMappingsComponent(NamJUCEAudioProcessor& p)
        : audioProcessor(p), entriesComp(p)
    {
        addAndMakeVisible(&viewport);
        viewport.setViewedComponent(&entriesComp, false);
        viewport.setScrollBarsShown(true, false, false, false);
    };

    ~MidiMappingsComponent()
    {

    };
    
    void paint(juce::Graphics& g) override
    {
        // g.fillAll(juce::Colours::red);
        g.setColour(juce::Colour::fromString("ff1e1e1e"));
        g.fillRoundedRectangle(topBarArea.toFloat(), 8.0f);

        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRect(titlesArea);

        g.setColour(juce::Colours::snow);

        int entryHeight = 30;
        int spacing = 20;
        // int width = titlesArea.proportionOfWidth(0.6) + entryHeight + (3*spacing);
        int width = titlesArea.proportionOfWidth(0.6) + (2*spacing);
        int x = getWidth() / 2 - width / 2;

        juce::Rectangle<int> r (x, titlesArea.getY(), proportionOfWidth(0.3f) - spacing, entryHeight);

        g.drawFittedText("Parameter", r, juce::Justification::centredBottom, 1);
        g.drawFittedText("CC", r.getX() + r.getWidth() + spacing, r.getY(), proportionOfWidth(0.15f) - spacing, r.getHeight(), juce::Justification::centredBottom, 1);
        g.drawFittedText("Channel", r.getX() + r.getWidth() + proportionOfWidth(0.15f) + spacing, r.getY(), proportionOfWidth(0.15f), r.getHeight(), juce::Justification::centredBottom, 1);

    };
    
    void resized() override
    {
        int barHeight = 50;
        int textAreaSize = 34;
        topBarArea.setBounds(0, 0, getWidth(), barHeight);
        titlesArea.setBounds(0, topBarArea.getY() + topBarArea.getHeight() - 5, getWidth(), textAreaSize);

        entriesComp.updateSize(this->getWidth()); // This doesn't make sense
        entriesComp.updateSize(this->getWidth());
        viewport.setBounds(0, titlesArea.getY() + titlesArea.getHeight() + 5, getWidth(), getHeight() - topBarArea.getHeight() - titlesArea.getHeight() - 5);

    };
    
private:
    NamJUCEAudioProcessor& audioProcessor;
    juce::Rectangle<int> topBarArea, listArea, titlesArea;
    MidiEntriesListComponent entriesComp;
    juce::Viewport viewport;
};

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
    juce::Rectangle<int> topBarArea, componentArea;
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

#endif // __MIDI_MAPPINGS_COMPONENT_H__
