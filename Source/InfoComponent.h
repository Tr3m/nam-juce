#ifndef __INFO_COMPONENT_H__
#define __INFO_COMPONENT_H__

#include <JuceHeader.h>
#include "LookAndFeel/LookAndFeel.h"
#include "Preferences/Preferences.h"

class InfoComponent : public juce::Component
{
public:
    InfoComponent(Preferences& pref)
        : preferences(pref)
    {
        lnf.setColour(juce::Label::ColourIds::textColourId, juce::Colours::snow);
        lnf.setRotarySliderImage(knobImage);
        lnf.setRotarySliderThumbScale(0.6f);
        lnf.setRotarySliderThumbOffset(11.0f);
        lnf.setColour(CoolButtons::Slider::ColourIds::thumbLedOffColourId,
                preferences.getColourSchemeColour(ColourScheme::knobThumbColourId));

        addAndMakeVisible(&knob);
        knob.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        knob.setLookAndFeel(&lnf);
        knob.setRange(0.0f, 1.0f, 0.01f);
        knob.setDoubleClickReturnValue(true, 0.5f);
        knob.setValue(0.5f,juce::NotificationType::dontSendNotification);

        addAndMakeVisible(&text);
        text.setLookAndFeel(&lnf);

        text.setText("NEURAL AMP MODELER\n(nam-juce)\n\nVersion " + juce::String(PLUG_VERSION)
                + "\n\nA JUCE implementation of the Neural Amp Modeler Plugin.",
                juce::NotificationType::dontSendNotification);

        text.setJustificationType(juce::Justification::centredTop);

        addAndMakeVisible(&linkButton);
        linkButton.setImages(false, true, false, favicon, 0.8f, juce::Colours::transparentBlack,
                favicon, 1.0f, juce::Colours::transparentBlack, favicon, 0.6f, juce::Colours::transparentBlack);
    
        linkButton.setTooltip(repoLink);
        linkButton.onClick = [this]
        {
            juce::URL repoUrl(repoLink);
            repoUrl.launchInDefaultBrowser();
        };
    };

    ~InfoComponent(){};

    void paint(juce::Graphics& g) override
    {
        g.drawImageWithin( knobBackground, knobArea.getX(),
                knobArea.getY(), knobArea.getWidth(), knobArea.getHeight(),
                juce::RectanglePlacement::centred);
    
    };

    void resized() override
    {
        int knobSize = 80;
        knobArea.setBounds(getWidth() / 2 - knobSize / 2, 10, knobSize, knobSize);
        knob.setBounds(knobArea.reduced(8));
        
        
        textArea.setBounds(10, knob.getY() + knob.getHeight() + 20, getWidth() - 20, knob.getHeight() + 45);
        text.setBounds(textArea);
        
        int faviconSize = 24;
        linkButton.setBounds(getWidth() / 2 - faviconSize / 2,
                textArea.getY() + textArea.getHeight() + 10, faviconSize, faviconSize);
    };

private:
    Preferences& preferences;
    
    juce::Slider knob;
    CoolSliderLookAndFeel lnf{CoolSliderLookAndFeel::RotarySliderDrawMethods::image};

    juce::Label text;
    
    juce::ImageButton linkButton;
    const juce::String repoLink {"https://github.com/tr3m/nam-juce"};

    juce::Image knobImage = juce::ImageFileFormat::loadFrom(BinaryData::knob_png, BinaryData::knob_pngSize);
    juce::Image knobBackground = juce::ImageFileFormat::loadFrom(BinaryData::knob_background_png, BinaryData::knob_background_pngSize);
    juce::Image favicon = juce::ImageFileFormat::loadFrom(BinaryData::githubicon_png, BinaryData::githubicon_pngSize);

    juce::Rectangle<int> knobArea, textArea;

    juce::TooltipWindow tooltipWindow{this, 300};

};
#endif
