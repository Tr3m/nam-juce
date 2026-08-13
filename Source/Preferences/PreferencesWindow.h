#ifndef __PREFERENCES_WINDOW_H__
#define __PREFERENCES_WINDOW_H__

#include <JuceHeader.h>
#include "PreferencesComponent.h"
#include "Preferences.h"


class PreferencesWindow : public juce::Component
{
public:

    PreferencesWindow(NamJUCEAudioProcessor& p, std::function<void ()>&& changeGUIColour)
        : preferencesComp(p, std::move(changeGUIColour)), audioProcessor(p)
        
    {
        addAndMakeVisible(&viewport);
        viewport.setViewedComponent(&preferencesComp, false);
        viewport.setScrollBarsShown(true, false, false, false);

        // this->updateColourScheme(false);
    };

    ~PreferencesWindow()
    {
        
    };

    void paint(juce::Graphics& g) override
    {
        // bgColour = audioProcessor.getPreferences().getColourScheme().secondary;
        // g.fillAll(bgColour);
    };

    void resized() override
    {
        preferencesComp.updateSize(getWidth()); 
        viewport.setBounds(getLocalBounds());
    };

private:
    juce::Colour bgColour;
    juce::LookAndFeel_V4 lnf;
    NamJUCEAudioProcessor& audioProcessor;
    juce::Viewport viewport;
    PreferencesComponent preferencesComp;
};



#endif

