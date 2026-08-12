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
    CoolSliderLookAndFeel(int);

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
    float rotarySliderTumbOffset {17.0f};
    float rotarySliderThumbScale {0.72f};

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

public:

    Slider::SliderLayout getSliderLayout(Slider& slider) override
    {
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth = jmax(0, jmin(slider.getTextBoxWidth(), localBounds.getWidth() - minXSpace));
        auto textBoxHeight = jmax(0, jmin(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth(textBoxWidth);
                layout.textBoxBounds.setHeight(textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)
                    layout.textBoxBounds.setX(0);
                else if (textBoxPos == Slider::TextBoxRight)
                    layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */
                    layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)
                    layout.textBoxBounds.setY(0);
                else if (textBoxPos == Slider::TextBoxBelow)
                    layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
                else /* left or right -> centre vertically */
                    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce(1, 1); // bar border
        }
        else
        {
            if (textBoxPos == Slider::TextBoxLeft)
                layout.sliderBounds.removeFromLeft(textBoxWidth);
            else if (textBoxPos == Slider::TextBoxRight)
                layout.sliderBounds.removeFromRight(textBoxWidth);
            else if (textBoxPos == Slider::TextBoxAbove)
                layout.sliderBounds.removeFromTop(textBoxHeight);
            else if (textBoxPos == Slider::TextBoxBelow)
                layout.sliderBounds.removeFromBottom(textBoxHeight + 15);

            const int thumbIndent = getSliderThumbRadius(slider);

            if (slider.isHorizontal())
                layout.sliderBounds.reduce(thumbIndent, 0);
            else if (slider.isVertical())
                layout.sliderBounds.reduce(0, thumbIndent);
        }

        return layout;
    }

    void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) override
    {
        if (textEditor.isEnabled())
        {
            if (textEditor.hasKeyboardFocus(true) && !textEditor.isReadOnly())
            {
                const int border = 2;

                // g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
                g.setColour(juce::Colours::transparentBlack);
                g.drawRect(0, 0, width, height, border);

                g.setOpacity(1.0f);
                auto shadowColour = textEditor.findColour(TextEditor::shadowColourId).withMultipliedAlpha(0.75f);
                // drawBevel (g, 0, 0, width, height + 2, border + 2, shadowColour, shadowColour);
            }
            else
            {
                g.setColour(textEditor.findColour(TextEditor::outlineColourId));
                g.drawRect(0, 0, width, height);

                g.setOpacity(1.0f);
                auto shadowColour = textEditor.findColour(TextEditor::shadowColourId);
                // drawBevel (g, 0, 0, width, height + 2, 3, shadowColour, shadowColour);
            }
        }
    }

};

#endif // __COOL_SLIDER_LNF_H__
