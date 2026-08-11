#ifndef __MIDI_MAPPINGS_COMPONENT_H__
#define __MIDI_MAPPINGS_COMPONENT_H__

#include <JuceHeader.h>
#include "../../PluginProcessor.h"
#include "MidiEntriesListComponent.h"
#include "../../LookAndFeel/LookAndFeel.h"
#include "MidiDialogBoxWrapper.h"

class MidiMappingsComponent : public juce::Component
{
public:
    MidiMappingsComponent(NamJUCEAudioProcessor& p, MidiHandler::EntryTypes type)
        : audioProcessor(p),type(type), entriesComp(p, type)
    {
        addAndMakeVisible(&viewport);
        viewport.setViewedComponent(&entriesComp, false);
        viewport.setScrollBarsShown(true, false, false, false);
        
        for (int i = 0; i < buttonLabels.size(); ++i)
        {
            buttons[i].reset(new juce::TextButton(buttonLabels[i]));
            addAndMakeVisible(buttons[i].get());
        }

        buttons[Buttons::New]->onClick = [this]
        {
            audioProcessor.getMidiHandler().clearMappings();
            entriesComp.reconstructUI();
        };

        buttons[Buttons::Open]->onClick = [this] { this->browseForPreset(); };

        buttons[Buttons::OpenDefault]->onClick = [this]
        {
            auto& mh = audioProcessor.getMidiHandler();
            mh.clearMappings();
            mh.loadConfig(mh.defaultMidiConfig, audioProcessor.apvts);
            entriesComp.reconstructUI();
        };

        buttons[Buttons::SaveAs]->onClick = [this] { showSaveDialog(); };
        buttons[Buttons::SaveAsDefault]->onClick = [this] 
        {
            auto& mh = audioProcessor.getMidiHandler();
            mh.saveConfig(mh.defaultMidiConfig);
        };


    };

    ~MidiMappingsComponent()
    {

    };
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRect(titlesArea);

        g.setColour(juce::Colours::snow);

        juce::Font fTemp = g.getCurrentFont();
        g.setFont(topBarArea.getHeight() / 3);
        g.drawFittedText("Midi Mappings", topBarArea.withY(topBarArea.getY() - 3), juce::Justification::centred, 1);
        g.setFont(fTemp);

        int entryHeight = 30;
        int spacing = 10;
        int width = titlesArea.proportionOfWidth(0.6) + (2*spacing) + entryHeight;
        int x = titlesArea.getX() + titlesArea.getWidth() / 2 - width / 2;

        juce::Rectangle<int> r (x, titlesArea.getY(), titlesArea.proportionOfWidth(0.3f) - spacing, entryHeight);

        g.drawFittedText(this->type == MidiHandler::EntryTypes::Parameter ? "Parameter" : "Preset", r, juce::Justification::centredBottom, 1);
        g.drawFittedText("CC", r.getX() + r.getWidth() + spacing, r.getY(), titlesArea.proportionOfWidth(0.15f) - spacing, r.getHeight(), juce::Justification::centredBottom, 1);
        g.drawFittedText("Channel", r.getX() + r.getWidth() + titlesArea.proportionOfWidth(0.15f) + spacing, r.getY(), titlesArea.proportionOfWidth(0.15f), r.getHeight(), juce::Justification::centredBottom, 1);

        g.setColour(juce::Colours::snow.withAlpha(0.2f));

        g.drawLine(buttons[Buttons::New]->getX() + buttons[Buttons::New]->getWidth() + 4,
                bottomBarArea.getY() + 2, buttons[Buttons::New]->getX() + buttons[Buttons::New]->getWidth() + 4,
                bottomBarArea.getY() + bottomBarArea.getHeight() - 2, 1.0f);

        g.drawLine(buttons[Buttons::OpenDefault]->getX() + buttons[Buttons::OpenDefault]->getWidth() + 4,
                bottomBarArea.getY() + 2, buttons[Buttons::OpenDefault]->getX() + buttons[Buttons::OpenDefault]->getWidth() + 4,
                bottomBarArea.getY() + bottomBarArea.getHeight() - 2, 1.0f);

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
    std::unique_ptr<MidiDialogBoxWrapper> saveDialog;
    MidiHandler::EntryTypes type;


    std::unique_ptr<juce::TextButton> buttons[5];
    const std::vector<juce::String> buttonLabels {"New", "Open", "Open Default", "Save As", "Save as Default"};

    void showSaveDialog()
    {
        if (saveDialog == nullptr)
        {
            saveDialog.reset(new MidiDialogBoxWrapper(audioProcessor, saveDialog));
            addAndMakeVisible(saveDialog.get());
            saveDialog->setBounds(getLocalBounds());
            saveDialog->grabKeyboardFocus();
        }
    }

    void browseForPreset()
    {
        auto& mh = audioProcessor.getMidiHandler();
        juce::FileChooser chooser("Choose an Preset to load", mh.midiDirectory, "*.xml", true, false);

        if (chooser.browseForFileToOpen())
        {
            mh.clearMappings();
            juce::File preset;
            preset = chooser.getResult();
            mh.loadConfig(preset, audioProcessor.apvts);
            entriesComp.reconstructUI();
        }
    };
};

#endif // __MIDI_MAPPINGS_COMPONENT_H__
