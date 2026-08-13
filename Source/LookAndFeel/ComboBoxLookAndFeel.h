#ifndef __COMBO_BOX_LNF_H__
#define __COMBO_BOX_LNF_H__

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
        juce::Rectangle<int> boxBounds (0, 0, width, height);

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
        juce::Rectangle<int> arrowZone (width - 30, 0, 20, height);
        Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

        g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath (path, PathStrokeType (2.0f));
        */
       
        }


};

class ComboBoxLNF : public juce::LookAndFeel_V4
{
public:
    ComboBoxLNF()
    {
    };

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                   int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {
        constexpr float cornerSize = 9.0f;
        juce::Rectangle<int> boxBounds (0, 0, width, height);
    
        g.setColour (box.findColour (ComboBox::backgroundColourId));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
    
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
    
        juce::Rectangle<int> arrowZone (width - 30, 0, 20, height);
        juce::Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);
    
        g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath (path, PathStrokeType (2.0f));
    }

    void drawPopupMenuBackgroundWithOptions(juce::Graphics& g, int width, int height,
            const juce::PopupMenu::Options& options) override
    {
        juce::ignoreUnused(options);

        constexpr float cornerRadius = 12.0f;

        g.setColour(findColour(juce::PopupMenu::backgroundColourId));

        #if JUCE_LINUX || JUCE_BSD 
        g.fillAll(findColour(juce::PopupMenu::backgroundColourId));
        #else
        g.fillRoundedRectangle(
            0.0f,
            0.0f,
            static_cast<float>(width),
            static_cast<float>(height),
            cornerRadius);
        #endif
    }

void drawPopupMenuItem (Graphics& g, const juce::Rectangle<int>& area,
                                        const bool isSeparator, const bool isActive,
                                        const bool isHighlighted, const bool isTicked,
                                        const bool hasSubMenu, const juce::String& text,
                                        const juce::String& shortcutKeyText,
                                        const juce::Drawable* icon, const juce::Colour* const textColourToUse) override
{
    constexpr float cornerRadius = 9.0f;

    if (isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (roundToInt (((float) r.getHeight() * 0.5f) - 0.5f));

        g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                                      : *textColourToUse);

        auto r  = area.reduced (1);

        if (isHighlighted && isActive)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRoundedRectangle (r.toFloat(), cornerRadius);

            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }

        r.reduce (jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float) r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();

        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft (roundToInt (maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape (1.0f);
            //g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
            g.fillEllipse(iconArea.withWidth(iconArea.getWidth() / 3).withHeight(iconArea.getWidth() / 3).withCentre(iconArea.getCentre()));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (text, r, Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}


};

#endif // __COMBO_BOX_LNF_H__
