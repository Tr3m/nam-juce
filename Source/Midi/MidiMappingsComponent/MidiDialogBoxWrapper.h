#ifndef __MIDI_DIALOG_BOX_WRAPPER_H__
#define __MIDI_DIALOG_BOX_WRAPPER_H__

#include <JuceHeader.h>
#include "MidiDialogBox.h"
#include "../../PluginProcessor.h"

class MidiDialogBoxWrapper : public juce::Component, private juce::MouseListener
{
public:

    MidiDialogBoxWrapper(NamJUCEAudioProcessor& p, std::unique_ptr<MidiDialogBoxWrapper>& selfRef)
        : self(selfRef)
    {
        dBox.reset(new MidiDialogBox(p, [&]() {destroy();}));
        addAndMakeVisible(dBox.get());
    };

    ~MidiDialogBoxWrapper()
    {
        dBox = nullptr;
        DBG("Goodbye Preset Dialog Box! (Destructor)");
    };

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.4f));

        // g.setColour(juce::Colours::white.withAlpha(0.6f));
        // g.drawRect(dBox->getBounds().withSizeKeepingCentre(dBox->getWidth() + 2, dBox->getHeight() + 2), 1);

        g.setColour(juce::Colour::fromString("ff252525"));
        g.fillRoundedRectangle(componentBounds.toFloat(), 8.0f);
    };

    void resized() override
    {
        int boxWidth = getWidth() / 3;
        int boxHeight = getHeight() / 3;
        componentBounds.setBounds(getWidth() / 2 - boxWidth / 2, getHeight() / 2 - boxHeight / 2, boxWidth, boxHeight);
        dBox->setBounds(componentBounds);
        repaint();
    };

    void destroy()
    {
       removeChildComponent(dBox.get());
       dBox = nullptr;

       DBG("Goodbye Preset Dialog Box!");
       self.reset();
    }

private:
    juce::Rectangle<int> componentBounds;
    std::unique_ptr<MidiDialogBox> dBox;
    std::unique_ptr<MidiDialogBoxWrapper>& self;

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.mouseWasClicked())
            destroy();
    };
};

#endif
