#ifndef __TEXT_BTN_LNF_H__
#define __TEXT_BTN_LNF_H__

using namespace juce;

class SimpleTextButtonLNF : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground (juce::Graphics&,
                               juce::Button&,
                               const juce::Colour&,
                               bool,
                               bool) override
    {
    }

    void drawButtonText (juce::Graphics& g,
                         juce::TextButton& button,
                         bool isMouseOver,
                         bool isButtonDown) override
    {
        float alpha = 0.75f;
        if (isMouseOver) alpha = 1.0f;
        if (isButtonDown) alpha = 0.5f;

        // g.setColour (juce::Colours::white.withAlpha (alpha));
        g.setColour(button.findColour(TextButton::ColourIds::textColourOnId).withAlpha(alpha));
        g.setFont (button.getHeight() - button.getHeight() / 4);
        g.drawText (button.getButtonText(),
                    button.getLocalBounds(),
                    juce::Justification::centred);
    }
};

class SimpleQuitButtonLNF : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground (juce::Graphics&,
                               juce::Button&,
                               const juce::Colour&,
                               bool,
                               bool) override
    {
    }

    void drawButtonText (juce::Graphics& g,
                         juce::TextButton& button,
                         bool isMouseOver,
                         bool isButtonDown) override
    {
        float alpha = 0.75f;
        if (isMouseOver)  alpha = 1.0f;
        if (isButtonDown) alpha = 0.5f;

        auto colour = button.findColour(juce::TextButton::ColourIds::textColourOnId).withAlpha(alpha);

        g.setColour(colour);

        auto bounds = button.getLocalBounds().toFloat().reduced(6.0f);

        auto xBounds = bounds.reduced(bounds.getWidth() * 0.15f,
                                      bounds.getHeight() * 0.15f);

        g.drawLine(xBounds.getTopLeft().x, xBounds.getTopLeft().y,
                   xBounds.getBottomRight().x, xBounds.getBottomRight().y,
                   2.0f);

        g.drawLine(xBounds.getTopRight().x, xBounds.getTopRight().y,
                   xBounds.getBottomLeft().x, xBounds.getBottomLeft().y,
                   2.0f);
        }
};

#endif //__TEXT_BTN_LNF_H__

