#pragma once
#include <JuceHeader.h>

class knobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    knobLookAndFeel()
    {
        knobImage = juce::ImageFileFormat::loadFrom(BinaryData::knob_png, BinaryData::knob_pngSize);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        if (knobImage.isValid())
        {
            const double rotation = (slider.getValue()
                - slider.getMinimum())
                / (slider.getMaximum()
                    - slider.getMinimum());

            const int frames = knobImage.getHeight() / knobImage.getWidth();
            const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
            const float radius = juce::jmin(width / 2.0f, height / 2.0f);
            const float centerX = x + width * 0.5f;
            const float centerY = y + height * 0.5f;
            const float rx = centerX - radius - 1.0f;
            const float ry = centerY - radius;

            g.drawImage(knobImage,
                (int)rx,
                (int)ry,
                2 * (int)radius,
                2 * (int)radius,
                0,
                frameId * knobImage.getWidth(),
                knobImage.getWidth(),
                knobImage.getWidth());
        }
        else
        {
            static const float textPpercent = 0.35f;
            juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f,
                0.5f * height, width * textPpercent, 0.5f * height);

            g.setColour(juce::Colours::white);

            g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
                juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
        }
    }

private:
    juce::Image knobImage;
};