#ifndef __NAM_KNOB_LNF_H__
#define __NAM_KNOB_LNF_H__

using namespace juce;

class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    enum KnobTypes
    {
        Main = 0,
        Minimal
    };

    KnobLookAndFeel(KnobTypes knobType)
    {
        switch (knobType)
        {
            case KnobTypes::Main: knobImage = juce::ImageFileFormat::loadFrom(BinaryData::knob_png, BinaryData::knob_pngSize); break;
            case KnobTypes::Minimal:
                knobImage = juce::ImageFileFormat::loadFrom(BinaryData::knob_minimal_png, BinaryData::knob_minimal_pngSize);
                break;
            default: knobImage = juce::ImageFileFormat::loadFrom(BinaryData::knob_png, BinaryData::knob_pngSize); break;
        }
    }

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

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        if (knobImage.isValid())
        {
            const double rotation = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());

            const int frames = knobImage.getHeight() / knobImage.getWidth();
            const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
            const float radius = juce::jmin(width / 2.0f, height / 2.0f);
            const float centerX = x + width * 0.5f;
            const float centerY = y + height * 0.5f;
            const float rx = centerX - radius - 1.0f;
            const float ry = centerY - radius;

            g.drawImage(knobImage, (int)rx, (int)ry, 2 * (int)radius, 2 * (int)radius, 0, frameId * knobImage.getWidth(), knobImage.getWidth(),
                        knobImage.getWidth());
        }
        else
        {
            static const float textPpercent = 0.35f;
            juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f, 0.5f * height, width * textPpercent, 0.5f * height);

            g.setColour(juce::Colours::white);

            g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
                             juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
        }
    }

private:
    juce::Image knobImage;
};

#endif //__NAM_KNOB_LNF_H__
