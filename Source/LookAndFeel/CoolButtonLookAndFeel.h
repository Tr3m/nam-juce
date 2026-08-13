#ifndef __COOL_BUTTON_LNF_H__
#define __COOL_BUTTON_LNF_H__

#include "ColourIds.h"
#include "../../Modules/melatonin_blur/melatonin_blur.h"

using namespace juce;

class CoolButtonLookAndFeel :  public juce::LookAndFeel_V4
{
public:
    CoolButtonLookAndFeel();
    CoolButtonLookAndFeel(bool);
    CoolButtonLookAndFeel(int, bool);

    void setColour (int colourId, Colour colour);
    juce::Colour findColour(int colourId);

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown) override;

    void setTextFont(const juce::Font&);
    juce::Font& getFont() { return this->textFont; };

    void setTextYOffset(int newOffset);

    void setGlowEnabled(bool);
    bool isGlowElabled() { return this->glowEnabled; };

    void setButtonStyle(int);

    void setButtonCornerRadius(int);

    void setOffsetDeficit(int, int);
    
    enum ButtonStyles
    {
        Rectangular = 0,
        Circular
    };
    
private:
    int buttonStyle {ButtonStyles::Rectangular};
    int buttonCornderRadius {6};
    bool glowEnabled {false};
    int offsetDeficitX {0};
    int offsetDeficitY {0};

    juce::Colour textColour {juce::Colour::fromString("FF969696")};
    float textSize {16.0f};
    juce::Font textFont {textSize, juce::Font::FontStyleFlags::bold};
    int textYOffset {0};

    juce::Colour shadowColor = juce::Colours::black.withAlpha(0.35f);
    int shadowRadius = 3;
    juce::Point<int> shadowOffset = { 2, 5 };
    int shadowSpread = 1;
    melatonin::DropShadow shadow = { shadowColor, shadowRadius, shadowOffset, shadowSpread };
    
    juce::Colour innerShadowColor = juce::Colours::snow.withAlpha(0.09f);
    int innerShadowRadius = 0;
    juce::Point<int> innerShadowOffset = { 1, 1 };
    int innerShadowSpread = 0;
    melatonin::InnerShadow innerShadow = { innerShadowColor, innerShadowRadius, innerShadowOffset, innerShadowSpread };

    juce::Colour innerColorPressedColour = juce::Colours::black.withAlpha(0.15f);
    int innerRadiusPressedRadius = 1;
    juce::Point<int> innerOffsetPressedOffset = { 0, 1 };
    int innerSpreadPressedSpread = 1;
    melatonin::InnerShadow innerShadowPressed = { innerColorPressedColour, innerRadiusPressedRadius, innerOffsetPressedOffset, innerSpreadPressedSpread };

    // Glow
    juce::Colour glowColour {juce::Colours::snow};
    int glowRadius = 5;
    juce::Point<int> glowOffset = { 0, 0 };
    int glowSpread = 1;
    melatonin::DropShadow outerGlow = { glowColour, glowRadius, glowOffset, glowSpread };

    void drawRectangularButton(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool);
    void drawCircularButton(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool);

    void drawRectangularButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown);
    void drawCircularButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown);

};

#endif // __COOL_BUTTON_LNF_H__
