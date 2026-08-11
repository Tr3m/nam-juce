#ifndef __COOL_SLIDER_LNF_H__
#define __COOL_SLIDER_LNF_H__

#include "ColourIds.h"
#include "../../Modules/melatonin_blur/melatonin_blur.h"

using namespace juce;

class CoolSliderLookAndFeel :  public juce::LookAndFeel_V4
{
public:
    CoolSliderLookAndFeel();
    CoolSliderLookAndFeel(int, bool);

    void setColour (int colourId, Colour colour);
    juce::Colour findColour(int colourId);

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float minSliderPos, float maxSliderPos,
            const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&) override;

    int getSliderThumbRadius (juce::Slider& slider) override;
    
    void setThumbStyle(int style);
    void setGlowEnabled(bool);
    void setInnerTrackGlowEnabled(bool);

    void setRotarySliderDrawMethod(int);
    int getRotarySliderDrawMethod() { return this->rotarySliderDrawMethod; };
    
    /*
     * Sets the offset of the rotary slider
     * dot/line relative to the center.
    */
    void setRotarySliderThumbOffset (float);
    int getRotarySliderThumbOffset() { return this->rotarySliderTumbOffset; };
    
    void setRotarySliderImage(const juce::Image&);

    enum ThumbStyles
    {
        regular = 0,
        withLed
    };

    enum RotarySliderDrawMethods
    {
        paint = 0,
        image,
        filmStrip
    };

private:
    int thumbStyle {ThumbStyles::withLed};
    int rotarySliderDrawMethod {RotarySliderDrawMethods::paint};
    float rotarySliderTumbOffset {10.0f};

    bool glowEnabled {false};
    bool trackInnerGlowEnabled {false};

    juce::Colour thumbColour {juce::Colour::fromString("ff2e2e2e")};

    juce::Colour rotarySliderColour {juce::Colour::fromString("ff2e2e2e")};

    juce::Colour color = juce::Colours::black.withAlpha(0.35f);
    int radius = 3;
    juce::Point<int> offset = { 2, 4 };
    int spread = 1;
    melatonin::DropShadow shadow = { color, radius, offset, spread };
    
    juce::Colour innerTrackShadowColor = juce::Colours::snow.withAlpha(0.2f);
    juce::Point<int> innerTrackShadowOffset = {0, -1};
    int innerTrackShadowRadius = 2;
    int innerTrackShadowSpread = 0;
    melatonin::InnerShadow innerTrackShadow = { innerTrackShadowColor, innerTrackShadowRadius, innerTrackShadowOffset, innerTrackShadowSpread };

    juce::Colour innerThumbShadowColor = juce::Colours::snow.withAlpha(0.15f);
    int innerThumbShadowRadius = 0;
    int innerThumbShadowSpread = 0;
    juce::Point<int> innerThumbShadowOffset = { 1, 1 };
    melatonin::InnerShadow innerThumbShadow = { innerThumbShadowColor, innerThumbShadowRadius, innerThumbShadowOffset, innerThumbShadowSpread };
    
    juce::Colour thumbLedBackgroundColour {juce::Colours::grey};

    // LED Glow
    juce::Colour glowColour {juce::Colours::snow};
    int glowRadius = 5;
    juce::Point<int> glowOffset = { 0, 0 };
    int glowSpread = 1;
    melatonin::DropShadow outerGlow = { glowColour, glowRadius, glowOffset, glowSpread };

    juce::Image knobImage;
    
    void paintRotarySlider(Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&);

    void paintRotarySliderWithImage(Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&);

};

#endif // __COOL_SLIDER_LNF_H__
