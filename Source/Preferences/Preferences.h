#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include <JuceHeader.h>

struct ColourScheme
{
    enum ColoursIds
    {
        titleColourId = 0,
        labelColourId,
        knobThumbColourId,
        ledColourId,
        eqLedColourId
    };

    ColourScheme(const juce::String& titleColour = "#ad7e55",
                 const juce::String& labelColour = "#ad7e55",
                 const juce::String& knobThumbColour = "#e8b600",
                 const juce::String& ledColour = "#ffb400",
                 const juce::String& eqLedColour = "#ffb400")
    {
        colours[ColoursIds::titleColourId] = juce::Colour(juce::Colour::fromString("#FF" + titleColour.replace("#", "", true)));
        colours[ColoursIds::labelColourId] = juce::Colour(juce::Colour::fromString("#FF" + labelColour.replace("#", "", true)));
        colours[ColoursIds::knobThumbColourId] = juce::Colour(juce::Colour::fromString("#FF" + knobThumbColour.replace("#", "", true)));
        colours[ColoursIds::ledColourId] = juce::Colour(juce::Colour::fromString("#FF" + ledColour.replace("#", "", true)));
        colours[ColoursIds::eqLedColourId] = juce::Colour(juce::Colour::fromString("#FF" + eqLedColour.replace("#", "", true)));
    }

    juce::Colour colours[5];
};

class Preferences
{
public:
    Preferences()
    {
        this->colourScheme = colours[colourSchemeIndex];
    }

    const std::vector<ColourScheme>& getColourSchemes() { return this->colours; };
    const ColourScheme getColourScheme() { return this->colourScheme; };

    const juce::Colour getColourSchemeColour(int index)
    {
        if (index < 0 || index > 5)
            return juce::Colour(juce::Colours::transparentBlack);
        
        return this->colourScheme.colours[index];

    }

    void setColourScheme(int index)
    {
        if (index < 0 || index > colours.size() - 1)
            index = 0;
        
        this->colourSchemeIndex = index;
        this->colourScheme = colours[index];
    }

    int getColourSchemeIndex() { return this->colourSchemeIndex; };
    
    const juce::Colour popupMenuColour {juce::Colour::fromString("FF121212").withAlpha(0.8f)};
    bool showA2Indicator {false};

private:
    ColourScheme colourScheme;
    int colourSchemeIndex {0};

    std::vector<ColourScheme> colours {
        {"#ad7e55", "#ad7e55", "#e8b600", "#ffb400", "#FF8700"},
        {"#11A884", "#11A884", "#0DB68F", "#1DFFCA", "#1DFFCA"},
        {"#903C9B", "#93439E", "#892496", "#A020FA", "#A020FA"},
        {"#98352E", "#A5322A", "#A00000", "#F22F2F", "#F22F2F"},
        {"#3B8C36", "#3B8C36", "#4AD242", "#31FF55", "#31FF55"},
        {"#2F608C", "#2F608C", "#387FBE", "#399CF5", "#399CF5"},
        {"#963A81", "#963A81", "#B92296", "#F539C9", "#F539C9"},
        {"#648429", "#648429", "#83BE17", "#B7FF34", "#B7FF34"}
    };
};

#endif
