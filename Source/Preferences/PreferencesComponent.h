#ifndef __PREFERENCES_COMPONENT_H__
#define __PREFERENCES_COMPONENT_H__

#include "../PluginProcessor.h"
#include "../LookAndFeel/LookAndFeel.h"

class PreferencesComponent : public juce::Component,
                            public juce::Slider::Listener,
                            public juce::ComboBox::Listener
{
public:

    PreferencesComponent(NamJUCEAudioProcessor& p, std::function<void()>&& setParentColour)
        : audioProcessor(p) ,preferences(p.getPreferences()), colourChanged(std::move(setParentColour))
    {
        auto preferences = audioProcessor.getPreferences();

        colourSchemeComboBox.reset(new juce::ComboBox());
        addAndMakeVisible(colourSchemeComboBox.get());
        
        auto colours = preferences.getColourSchemes();
        auto currentColour = preferences.getColourScheme();

        for (int i = 0; i < colours.size(); ++i)
            colourSchemeComboBox->addItem(juce::String(colours[i].colours[ColourScheme::ColoursIds::eqLedColourId].toString()), i+1);
        colourSchemeComboBox->setLookAndFeel(&lnf);
        colourSchemeComboBox->setText(juce::String(currentColour.colours[ColourScheme::ColoursIds::eqLedColourId].toString()), juce::NotificationType::dontSendNotification);

        colourSchemeComboBox->addListener(this);

        colourSchemeLabel.reset(new juce::Label({}, TRANS("Colour Scheme:")));
        addAndMakeVisible(colourSchemeLabel.get());
        colourSchemeLabel->setJustificationType(juce::Justification::centredRight);
        colourSchemeLabel->attachToComponent(colourSchemeComboBox.get(), true);

        a2Indicator.reset(new juce::ToggleButton());
        addAndMakeVisible(a2Indicator.get());
        a2Indicator->setToggleState(preferences.showA2Indicator, juce::NotificationType::dontSendNotification);
        a2Indicator->onClick = [this]
        {
            auto& pref = audioProcessor.getPreferences();
            pref.showA2Indicator = a2Indicator->getToggleState();
            colourChanged();
        };

        a2IndicatorLabel.reset(new juce::Label({}, TRANS("Show A2 Indicator\nin Model Text Box:")));
        a2IndicatorLabel->setJustificationType(juce::Justification::centredRight);
        a2IndicatorLabel->attachToComponent(a2Indicator.get(), true);

        lnf.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::snow.withAlpha(0.5f));
        lnf.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        lnf.setColour(juce::PopupMenu::ColourIds::backgroundColourId, preferences.popupMenuColour);
    };

    ~PreferencesComponent() 
    {
    };

    void paint(juce::Graphics& g) override
    {
        // g.fillAll(preferences.getColourScheme().secondary);
    };


    void resized() override
    {
        // DBG("Child Comp, W:" + juce::String(getWidth()) + ", H:" + juce::String(getHeight()));

        componentsArea.setBounds(proportionOfWidth (0.35f), 15, proportionOfWidth (0.6f), 3000);
        juce::Rectangle<int> r (proportionOfWidth (0.35f), 15, proportionOfWidth (0.6f), 3000);
        
        if (colourSchemeComboBox != nullptr)
        {
            colourSchemeComboBox->setBounds(r.removeFromTop(optionEntryHeight));
            r.removeFromTop(spacing);
        }

        if (a2Indicator != nullptr)
        {
            a2Indicator->setBounds(r.removeFromTop(optionEntryHeight + 10));
            r.removeFromTop(spacing);
        }

    };

    int getLowestY() const
    {
        int y = 0;

        for (auto* c : getChildren())
            y = jmax (y, c->getBottom());

        return y;
    }


    /*
    * TODO: Remove parent heigt and add the height of 
    the last compoenent when implemented.
    */
    void updateSize(int parentWidth) 
    {
        this->setSize(parentWidth, this->getLowestY());
    }

    void sliderValueChanged(juce::Slider* slider) override
    {
    }

    
    void comboBoxChanged (ComboBox* cb) override
    {
        if (cb == colourSchemeComboBox.get())
        {
            preferences.setColourScheme(colourSchemeComboBox->getSelectedId() - 1);

            auto colourScheme = preferences.getColourScheme();
            
            colourSchemeComboBox->setSelectedId(0, juce::NotificationType::dontSendNotification);
            colourSchemeComboBox->setText(juce::String(colourScheme.colours[ColourScheme::ColoursIds::eqLedColourId].toString()),
                    juce::NotificationType::dontSendNotification);

            colourChanged();
        }
    };

private:
    Preferences& preferences;
    int spacing = 5;
    juce::Rectangle<int> componentsArea;

    std::unique_ptr<juce::ComboBox> colourSchemeComboBox;
    std::unique_ptr<juce::ToggleButton> a2Indicator;
    int optionEntryHeight = 30;

    std::unique_ptr<juce::Label> colourSchemeLabel, a2IndicatorLabel;

    juce::TooltipWindow tooltipWindow{this, 300};

    std::function<void()> colourChanged;
    ColourComboBoxLNF lnf;

    NamJUCEAudioProcessor& audioProcessor;

};

#endif
