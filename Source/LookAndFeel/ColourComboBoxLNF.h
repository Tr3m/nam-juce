#ifndef __COLOUR_COMBO_BOX_LNF_H__
#define __COLOUR_COMBO_BOX_LNF_H__

using namespace juce;

class ColourComboBoxLNF : public juce::LookAndFeel_V4
{
public:

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator,
                               bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                               const juce::String& text, const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColour) override
    {
        if (isSeparator)
            return;

        if (isHighlighted)
            g.fillAll(juce::Colours::lightblue);
        
        auto colour = juce::Colour::fromString(text);

        auto square = area.reduced(6);

        g.setColour(colour);
        g.fillRect(square);
        g.setColour(juce::Colours::black);
        g.drawRect(square);
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                   int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {
        //Background and border
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (box.findColour (ComboBox::backgroundColourId));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
       
        box.setColour(juce::ComboBox::textColourId, juce::Colours::transparentBlack);

        // My stuff
        auto colour = juce::Colour::fromString(box.getText());

        juce::Rectangle<int> area (width, height);

        auto square = area.reduced(6);

        g.setColour(colour);
        g.fillRect(square);

        g.setColour(juce::Colours::black);
        g.drawRect(square);

        // Draw the arrow...
        /*
        Rectangle<int> arrowZone (width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

        g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath (path, PathStrokeType (2.0f));
        */
       
        }


};

#endif // __COLOUR_COMBO_BOX_LNF_H__
