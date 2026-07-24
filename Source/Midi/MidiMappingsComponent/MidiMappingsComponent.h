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
        
        for (int i = 0; i < buttonLabels.size(); ++i)
        {
            buttons[i].reset(new juce::TextButton(buttonLabels[i]));
            addAndMakeVisible(buttons[i].get());
        }
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

        juce::Font fTemp = g.getCurrentFont();
        // g.setFont(juce::Font(topBarArea.getHeight() / 2, juce::Font::FontStyleFlags::bold));
        g.setFont(topBarArea.getHeight() / 3);
        g.drawFittedText("MIDI Mappings", topBarArea/*.withX(topBarArea.getX() + 20)*/, juce::Justification::centred, 1);
        g.setFont(fTemp);

        int entryHeight = 30;
        int spacing = 20;
        int width = titlesArea.proportionOfWidth(0.6) + (2*spacing);
        int x = getWidth() / 2 - width / 2;

        juce::Rectangle<int> r (x, titlesArea.getY(), proportionOfWidth(0.3f) - spacing, entryHeight);

        g.drawFittedText("Parameter", r, juce::Justification::centredBottom, 1);
        g.drawFittedText("CC", r.getX() + r.getWidth() + spacing, r.getY(), proportionOfWidth(0.15f) - spacing, r.getHeight(), juce::Justification::centredBottom, 1);
        g.drawFittedText("Channel", r.getX() + r.getWidth() + proportionOfWidth(0.15f) + spacing, r.getY(), proportionOfWidth(0.15f), r.getHeight(), juce::Justification::centredBottom, 1);

        g.setColour(juce::Colour::fromString("ff1e1e1e"));
        g.fillRoundedRectangle(bottomBarArea.toFloat(), 8.0f);
        g.fillRect(bottomBarArea.withY(bottomBarArea.getY()).withHeight(5));

        g.setColour(juce::Colours::snow.withAlpha(0.2f));

        g.drawLine(buttons[Buttons::New]->getX() + buttons[Buttons::New]->getWidth() + 4,
                bottomBarArea.getY() + 2, buttons[Buttons::New]->getX() + buttons[Buttons::New]->getWidth() + 4,
                bottomBarArea.getY() + bottomBarArea.getHeight() - 2, 1.0f);

        g.drawLine(buttons[Buttons::OpenDefault]->getX() + buttons[Buttons::OpenDefault]->getWidth() + 4,
                bottomBarArea.getY() + 2, buttons[Buttons::OpenDefault]->getX() + buttons[Buttons::OpenDefault]->getWidth() + 4,
                bottomBarArea.getY() + bottomBarArea.getHeight() - 2, 1.0f);

    };
    
    void resized() override
    {
        int barHeight = 50;
        int textAreaSize = 34;
        int bottomBarHeight = 30;

        topBarArea.setBounds(0, 0, getWidth(), barHeight);
        titlesArea.setBounds(0, topBarArea.getY() + topBarArea.getHeight() - 5, getWidth(), textAreaSize);
        bottomBarArea.setBounds(0, getHeight() - bottomBarHeight, getWidth(), bottomBarHeight);

        entriesComp.updateSize(this->getWidth()); // This doesn't make sense
        entriesComp.updateSize(this->getWidth());
        viewport.setBounds(0, titlesArea.getY() + titlesArea.getHeight() + 5, getWidth(), getHeight() - topBarArea.getHeight() - titlesArea.getHeight() - bottomBarHeight);
        
        int buttonWidth = 50;
        int buttonHeight = bottomBarArea.getHeight() - bottomBarArea.getHeight() / 3;
        int buttonY = bottomBarArea.getY() + bottomBarArea.getHeight() - bottomBarArea.getHeight() / 2 - buttonHeight / 2;
        int buttonSpacing = 8;

        buttons[Buttons::New]->setBounds(bottomBarArea.getX(), buttonY, buttonWidth, buttonHeight);
        buttons[Buttons::New]->changeWidthToFitText();

        buttons[Buttons::Open]->setBounds(buttons[Buttons::New]->getX() + buttons[Buttons::New]->getWidth() + buttonSpacing, 
                buttons[Buttons::New]->getY(), buttons[Buttons::New]->getWidth(), buttons[Buttons::New]->getHeight());

        buttons[Buttons::Open]->changeWidthToFitText();

        buttons[Buttons::OpenDefault]->setBounds(buttons[Buttons::Open]->getX() + buttons[Buttons::Open]->getWidth() + buttonSpacing, 
                buttons[Buttons::Open]->getY(), buttons[Buttons::Open]->getWidth() + buttonWidth / 2 + 10, buttons[Buttons::Open]->getHeight());

        buttons[Buttons::OpenDefault]->changeWidthToFitText();

        buttons[Buttons::SaveAs]->setBounds(buttons[Buttons::OpenDefault]->getX() + buttons[Buttons::OpenDefault]->getWidth() + buttonSpacing, 
                buttons[Buttons::OpenDefault]->getY(), buttons[Buttons::OpenDefault]->getWidth(), buttons[Buttons::OpenDefault]->getHeight());
        
        buttons[Buttons::SaveAs]->changeWidthToFitText();

        buttons[Buttons::SaveAsDefault]->setBounds(buttons[Buttons::SaveAs]->getX() + buttons[Buttons::SaveAs]->getWidth() + buttonSpacing, 
                buttons[Buttons::SaveAs]->getY(), buttons[Buttons::SaveAs]->getWidth() + buttonWidth / 2, buttons[Buttons::SaveAs]->getHeight());

        buttons[Buttons::SaveAsDefault]->changeWidthToFitText();
    };

    enum Buttons
    {
        New = 0,
        Open,
        OpenDefault,
        SaveAs,
        SaveAsDefault
    };
    
private:
    NamJUCEAudioProcessor& audioProcessor;
    juce::Rectangle<int> topBarArea, listArea, titlesArea, bottomBarArea;
    MidiEntriesListComponent entriesComp;
    juce::Viewport viewport;


    std::unique_ptr<juce::TextButton> buttons[5];
    const std::vector<juce::String> buttonLabels {"New", "Open", "Open Default", "Save As", "Save as Default"};
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

#endif // __MIDI_MAPPINGS_COMPONENT_H__
