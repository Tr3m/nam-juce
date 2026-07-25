#ifndef __PROGRAM_CHANGE_MAPPINGS_COMPONENT_H__
#define __PROGRAM_CHANGE_MAPPINGS_COMPONENT_H__

#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "ProgramChangeListComponent.h"
#include "../../MyLookAndFeel.h"

class ProgramChangeMappingsComponent : public juce::Component
{
public:
    ProgramChangeMappingsComponent(NamJUCEAudioProcessor& p)
        : audioProcessor(p), entriesComp(audioProcessor.getAllPresets())
    {
        addAndMakeVisible(&viewport);
        viewport.setViewedComponent(&entriesComp, false);
        viewport.setScrollBarsShown(true, false, false, false);
        
    };

    ~ProgramChangeMappingsComponent()
    {

    };
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRect(titlesArea);

        g.setColour(juce::Colours::snow);

        juce::Font fTemp = g.getCurrentFont();
        g.setFont(topBarArea.getHeight() / 3);
        g.drawFittedText("Progam Change Mappings", topBarArea.withY(topBarArea.getY() - 3), juce::Justification::centred, 1);
        g.setFont(fTemp);

        int entryHeight = 30;
        int spacing = 10;
        int labelWidth = titlesArea.proportionOfWidth(0.1f);
        int width = titlesArea.proportionOfWidth(0.5f) + spacing;
        int x = titlesArea.getX() + titlesArea.getWidth() / 2 - width / 2 + labelWidth + (spacing / 2);

        juce::Rectangle<int> r (x, titlesArea.getY(), titlesArea.proportionOfWidth(0.4f) - spacing, entryHeight);

        g.drawFittedText("Preset", r, juce::Justification::centredBottom, 1);
        g.drawFittedText("PC#", r.getX() - labelWidth - (spacing / 2) + 2, titlesArea.getY(), labelWidth, entryHeight, juce::Justification::centredBottom, 1);

        // g.fillAll(juce::Colours::red);

    };
    
    void resized() override
    {
        int barHeight = 50;
        int textAreaSize = 34;
        int bottomBarHeight = 30;

        topBarArea.setBounds(0, 0, getWidth(), barHeight);
        titlesArea.setBounds(proportionOfWidth(0.2), topBarArea.getY() + topBarArea.getHeight() - 5, getWidth() - proportionOfWidth(0.2), textAreaSize);
        bottomBarArea.setBounds(0, getHeight() - bottomBarHeight, getWidth(), bottomBarHeight);

        entriesComp.updateSize(titlesArea.getWidth()); // This doesn't make sense
        entriesComp.updateSize(titlesArea.getWidth());
        viewport.setBounds(titlesArea.getX(), titlesArea.getY() + titlesArea.getHeight() + 5, titlesArea.getWidth(), getHeight() - topBarArea.getHeight() - titlesArea.getHeight() - bottomBarHeight);
        
    };

    
private:
    NamJUCEAudioProcessor& audioProcessor;
    juce::Rectangle<int> topBarArea, listArea, titlesArea, bottomBarArea;
    ProgramChangeListComponent entriesComp;
    juce::Viewport viewport;
    // std::unique_ptr<MidiDialogBoxWrapper> saveDialog;

};

#endif // __PROGRAM_CHANGE_MAPPINGS_COMPONENT_H__
