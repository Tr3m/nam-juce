#ifndef __MIDI_MAPPINGS_COMPONENT_WRAPPER_H__
#define __MIDI_MAPPINGS_COMPONENT_WRAPPER_H__

#include "MidiMappingsComponent.h"
#include "ProgramChangeMappingsComponent.h"

class MidiMappingsComponentWrapper : public juce::Component,
                                     public juce::MouseListener
{
public:
    
    MidiMappingsComponentWrapper(NamJUCEAudioProcessor& p, std::unique_ptr<MidiMappingsComponentWrapper>& selfRef)
        : self(selfRef), audioProcessor(p)
    {
        lnf.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        lnf.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        lnf.setColour(juce::PopupMenu::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
        lnf.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::black.withAlpha(0.7f));

        mappingsComp.reset(new MidiMappingsComponent(p, MidiHandler::EntryTypes::Parameter));
        mappingsComp->setLookAndFeel(&lnf);
        addAndMakeVisible(mappingsComp.get());
        
        closeButton.reset(new juce::TextButton("X"));
        addAndMakeVisible(closeButton.get());
        closeButton->setLookAndFeel(&lnf);
        closeButton->onClick = [this] { this->destroy(); };
        closeButton->setAlwaysOnTop(true);

        ccButton.reset(new juce::TextButton("Parameters"));
        addAndMakeVisible(ccButton.get());
        ccButton->setLookAndFeel(&lnf);
        ccButton->onClick = [this] { ccMappingsClicked(); };
        ccButton->setAlwaysOnTop(true);

        pcButton.reset(new juce::TextButton("Presets"));
        addAndMakeVisible(pcButton.get());
        pcButton->setLookAndFeel(&lnf);
        pcButton->onClick = [this] { pcMappingsClicked(); };
        pcButton->setAlwaysOnTop(true);
    }
    
    ~MidiMappingsComponentWrapper()
    {
        mappingsComp = nullptr;
        DBG("Goodbye MIDI Mappings (Dectructor)...");
    };

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.7f));
        
        // Window Area
        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRoundedRectangle(windowArea.toFloat(), 8.0f);
    
        // Top bar
        g.setColour(juce::Colour::fromString("ff1e1e1e"));
        g.fillRoundedRectangle(topBarArea.toFloat(), 8.0f);
        g.fillRect(topBarArea.withY(topBarArea.getY() + topBarArea.getHeight() - 5).withHeight(5));
        
        // Bottom Bar
        g.fillRoundedRectangle(bottomBarArea.toFloat(), 8.0f);
        g.fillRect(bottomBarArea.withY(bottomBarArea.getY()).withHeight(5));

        // Side bar
        g.fillRoundedRectangle(sideBarArea.toFloat(), 8.0);

        g.setColour(juce::Colours::ivory.withAlpha(0.35f));

        if (mappingsComp != nullptr)
            g.fillRoundedRectangle(ccButton->getBounds().toFloat().withWidth(sideBarArea.getWidth() - 20).withX(sideBarArea.getX() + 10), 8.0f);
        else
            g.fillRoundedRectangle(pcButton->getBounds().toFloat().withWidth(sideBarArea.getWidth() - 20).withX(sideBarArea.getX() + 10), 8.0f);
    };

    void resized() override
    {
        int width = int(0.8 * getWidth());
        int height = int(0.8 * getHeight());

        windowArea.setBounds(getWidth() / 2 - width / 2, getHeight() / 2 - height / 2, width, height);

        int barHeight = 40;
        int textAreaSize = 34;
        int bottomBarHeight = 30;

        topBarArea.setBounds(windowArea.getX(), windowArea.getY(), windowArea.getWidth(), barHeight);
        bottomBarArea.setBounds(windowArea.getX(), windowArea.getHeight() + windowArea.getY() - bottomBarHeight, windowArea.getWidth(), bottomBarHeight);
        sideBarArea.setBounds(windowArea.getX(), windowArea.getY(), windowArea.proportionOfWidth(0.2), windowArea.getHeight());

        if (mappingsComp != nullptr)
            mappingsComp->setBounds(windowArea);

        int buttonSize = 30;
        closeButton->setBounds(windowArea.getWidth() + windowArea.getX() - buttonSize - 5, windowArea.getY() + 7, buttonSize, buttonSize);
        
        int sideBarButtonWidth = sideBarArea.getWidth() - sideBarArea.getWidth() / 3;
        ccButton->setBounds(sideBarArea.getX() + sideBarArea.getWidth() / 2 - sideBarButtonWidth / 2, sideBarArea.proportionOfHeight(0.3f), sideBarButtonWidth, 24);
        pcButton->setBounds(ccButton->getX(), ccButton->getY() + ccButton->getHeight() + 10, ccButton->getWidth(), ccButton->getHeight());
    };

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.mouseWasClicked() && !windowArea.contains(event.getPosition()))
            this->destroy();
    };

private:
    juce::Rectangle<int>topBarArea, windowArea, bottomBarArea, sideBarArea;
    std::unique_ptr<MidiMappingsComponent> mappingsComp, pcMappingsComp;
    std::unique_ptr<MidiMappingsComponentWrapper>& self;
    std::unique_ptr<juce::TextButton> closeButton, ccButton, pcButton;
    SimpleTextButtonLNF lnf;

    NamJUCEAudioProcessor& audioProcessor;

    void destroy()
    {
        mappingsComp = nullptr;
        self.reset();

        DBG("Goodbye MIDI Mappings...");
    };

    void ccMappingsClicked()
    {
        if (pcMappingsComp != nullptr)
        {
            this->removeChildComponent(pcMappingsComp.get());
            pcMappingsComp.reset();
        }

        if (mappingsComp == nullptr)
        {
            mappingsComp.reset(new MidiMappingsComponent(audioProcessor, MidiHandler::EntryTypes::Parameter));
            addAndMakeVisible(mappingsComp.get());
            mappingsComp->setLookAndFeel(&lnf);
            mappingsComp->setBounds(windowArea);
        }

        repaint();
    };

    void pcMappingsClicked()
    {
        if (mappingsComp != nullptr)
        {
            this->removeChildComponent(mappingsComp.get());
            mappingsComp.reset();
        }

        if (pcMappingsComp == nullptr)
        {
            pcMappingsComp.reset(new MidiMappingsComponent(audioProcessor, MidiHandler::EntryTypes::Preset));
            addAndMakeVisible(pcMappingsComp.get());
            pcMappingsComp->setLookAndFeel(&lnf);
            pcMappingsComp->setBounds(windowArea);
        }

        repaint();
    };
};

#endif //__MIDI_MAPPINGS_COMPONENT_WRAPPER_H__
