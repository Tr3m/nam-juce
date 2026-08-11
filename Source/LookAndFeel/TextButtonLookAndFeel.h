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

#endif //__TEXT_BTN_LNF_H__
