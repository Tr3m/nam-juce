#include "CoolButtonLookAndFeel.h"


CoolButtonLookAndFeel::CoolButtonLookAndFeel()
{
    this->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromString("ff2e2e2e"));
    this->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour::fromString("FF969696"));
    this->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour::fromString("FF969696"));
}

CoolButtonLookAndFeel::CoolButtonLookAndFeel(bool glowEnabled)
{
    this->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromString("ff2e2e2e"));
    this->setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colour::fromString("FF969696"));
    this->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour::fromString("FF969696"));
    this->glowEnabled = glowEnabled;
}

CoolButtonLookAndFeel::CoolButtonLookAndFeel(int buttonStyle, bool glowEnabled)
{
    CoolButtonLookAndFeel();
    this->glowEnabled = glowEnabled;
    
    switch (buttonStyle)
    {
        case ButtonStyles::Rectangular:
            this->buttonStyle = ButtonStyles::Rectangular;
            break;
        case ButtonStyles::Circular:
            this->buttonStyle = ButtonStyles::Circular;
            break;
        default:
            this->buttonStyle = ButtonStyles::Rectangular;
            break;
    }
}

void CoolButtonLookAndFeel::setColour (int colourId, juce::Colour colour)
{
    switch (colourId)
    {
        case (CoolButtons::TextButton::ColourIds::textGlowColourId):
            this->glowColour = colour;
            break;
        default:
            LookAndFeel_V4::setColour(colourId, colour);
            break;
    }
}


juce::Colour CoolButtonLookAndFeel::findColour(int colourId)
{
    switch (colourId)
    {
        case CoolButtons::TextButton::ColourIds::textGlowColourId:
            return this->glowColour;
            break;
        default:
            return LookAndFeel_V4::findColour(colourId);
            break;
    }
}

void CoolButtonLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool drawHighlighted, bool isDown)
{
    switch (this->buttonStyle)
    {
    case ButtonStyles::Rectangular:
        drawRectangularButton(g, button, backgroundColour, drawHighlighted, isDown);
        break;
    case ButtonStyles::Circular:
        drawCircularButton(g, button, backgroundColour, drawHighlighted, isDown);
        break;
    }
}


void CoolButtonLookAndFeel::drawRectangularButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool drawHighlighted, bool isDown)
{
    g.setColour(juce::Colours::black.withAlpha(0.55f));

    juce::Path valueTrack;
    juce::Rectangle<int> bounds (2, 2, button.getWidth() - shadowOffset.x - shadowSpread - shadowRadius - 2,
            button.getHeight() - shadowOffset.y - shadowSpread - shadowRadius - 2);

    valueTrack.addRoundedRectangle(bounds, this->buttonCornderRadius);

    if (isDown)
    {
        innerShadowOffset = {0, -2 - this->offsetDeficitY};
        innerShadowRadius = 0;
        innerShadowSpread = 0;
        innerShadow = { innerShadowColor, innerShadowRadius, innerShadowOffset, innerShadowSpread };
    }
    else
    {
        innerShadowOffset = {2 + this->offsetDeficitX, 2 + this->offsetDeficitY};
        innerShadowRadius = 0;
        innerShadowSpread = 0;
        innerShadow = { innerShadowColor, innerShadowRadius, innerShadowOffset, innerShadowSpread };
    }

    float holeSize = .5f;

    // drop shadows get painted *before* the path
    if (!isDown) 
        shadow.render(g, valueTrack);
    else
    {
        // g.fillRoundedRectangle(bounds.getX() - holeSize,
        //         bounds.getY() - holeSize,
        //         bounds.getWidth() + 2 * holeSize,
        //         bounds.getHeight() + 2 * holeSize, this->buttonCornderRadius);
    }

    g.setColour (backgroundColour);
    g.fillPath (valueTrack);


    // inner shadows are painted *after* the path
    innerShadow.render(g, valueTrack);

    if (isDown)
        innerShadowPressed.render(g, valueTrack);
}

void CoolButtonLookAndFeel::drawCircularButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool drawHighlighted, bool isDown)
{
    g.setColour(juce::Colours::black.withAlpha(0.55f));

    auto bounds = Rectangle<int> (button.getX(), button.getY(), button.getWidth(),
            button.getHeight()).withCentre(juce::Point(button.getWidth() / 2, button.getHeight() / 2)).toFloat().reduced (6);

    bounds = bounds.withX(bounds.getX() - shadowOffset.x / 2).withY(bounds.getY() - shadowOffset.y / 2);

    int holeSize = 2;
    auto holeBounds = bounds.withSizeKeepingCentre(bounds.getWidth() + holeSize, bounds.getHeight() + holeSize);

    juce::Path valueTrack;

    valueTrack.addEllipse(bounds);

    if (isDown)
    {
        innerShadowOffset = {0, -1};
        innerShadowRadius = 2;
        innerShadowSpread = 0;
        innerShadow = { innerShadowColor, innerShadowRadius, innerShadowOffset, innerShadowSpread };
    }
    else
    {
        innerShadowOffset = {1, 1};
        innerShadowRadius = 0;
        innerShadowSpread = 0;
        innerShadow = { innerShadowColor, innerShadowRadius, innerShadowOffset, innerShadowSpread };
    }

    // drop shadows get painted *before* the path
    if (!isDown) 
        shadow.render(g, valueTrack);
    else
        g.fillEllipse(holeBounds);

    g.setColour (backgroundColour);
    g.fillPath (valueTrack);


    // inner shadows are painted *after* the path
    innerShadow.render(g, valueTrack);

    if (isDown)
        innerShadowPressed.render(g, valueTrack);
}

void CoolButtonLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown)
{
    switch (this->buttonStyle)
    {
        case ButtonStyles::Rectangular:
            drawRectangularButtonText(g, button, isMouseOver, isButtonDown);
            break;
        case ButtonStyles::Circular: 
            drawCircularButtonText(g, button, isMouseOver, isButtonDown);
            break;
    }
}


void CoolButtonLookAndFeel::drawRectangularButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown)
{
    juce::Rectangle<int> bounds (2, 2, button.getWidth() - shadowOffset.x - shadowSpread - shadowRadius - 2,
            button.getHeight() - shadowOffset.y - shadowSpread - shadowRadius - 2);

    float yOffsetLocal = isButtonDown ? 0 : 1.8;

    g.setFont(textFont);
    g.setColour(this->findColour(button.getToggleState() ? juce::TextButton::ColourIds::textColourOnId : juce::TextButton::ColourIds::textColourOffId));

    if (glowEnabled)
    {
        outerGlow = { glowColour.withAlpha(0.4f), glowRadius, glowOffset, glowSpread };

        g.setColour(glowColour);
        outerGlow.render(g, button.getButtonText(), bounds.withX(bounds.getX() + 1)
                .withY(bounds.getY() + yOffsetLocal + textYOffset),
                juce::Justification::centred);

        // g.setColour(glowColour.withAlpha(0.7f));
    }
        
    g.drawFittedText(button.getButtonText(), bounds.withX(bounds.getX() + 1)
            .withY(bounds.getY() + yOffsetLocal + textYOffset),
            juce::Justification::centred, 1);
}

void CoolButtonLookAndFeel::drawCircularButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOver, bool isButtonDown)
{

    auto bounds = juce::Rectangle<int> (button.getX(), button.getY(), button.getWidth(),
            button.getHeight()).withCentre(juce::Point(button.getWidth() / 2, button.getHeight() / 2)).reduced (6);

    bounds = bounds.withX(bounds.getX() - shadowOffset.x / 2).withY(bounds.getY() - shadowOffset.y / 2);

    float yOffsetLocal = isButtonDown ? 0 : 1.8;
    
    this->textSize = button.getHeight() / 6 - 1;
    textFont = juce::Font(textSize, juce::Font::FontStyleFlags::bold);
    g.setFont(textFont);
    g.setColour(this->findColour(button.getToggleState() ? juce::TextButton::ColourIds::textColourOnId : juce::TextButton::ColourIds::textColourOffId));
    

    if (glowEnabled)
    {
        outerGlow = { glowColour, glowRadius, glowOffset, glowSpread };

        g.setColour(glowColour);
        outerGlow.render(g, button.getButtonText(), bounds.withX(bounds.getX() + 1)
                .withY(bounds.getY() + yOffsetLocal + textYOffset),
                juce::Justification::centred);

        // g.setColour(glowColour.withAlpha(0.7f));
    }

    g.drawFittedText(button.getButtonText(), bounds.withX(bounds.getX() + 1)
            .withY(bounds.getY() + yOffsetLocal + textYOffset),
            juce::Justification::centred, 1);

}


void CoolButtonLookAndFeel::setTextFont(const juce::Font& newFont)
{
    this->textFont = newFont;
}

void CoolButtonLookAndFeel::setTextYOffset(int newOffset)
{
    this->textYOffset = newOffset;
}

void CoolButtonLookAndFeel::setGlowEnabled(bool shouldGlowBeEnabled)
{
    this->glowEnabled = shouldGlowBeEnabled;
}

void CoolButtonLookAndFeel::setButtonStyle(int buttonStyle)
{
    switch (buttonStyle)
    {
        case ButtonStyles::Rectangular:
            this->buttonStyle = ButtonStyles::Rectangular;
            break;
        case ButtonStyles::Circular:
            this->buttonStyle = ButtonStyles::Circular;
            break;
        default:
            this->buttonStyle = ButtonStyles::Rectangular;
            break;
    }
}

void CoolButtonLookAndFeel::setButtonCornerRadius(int newRadius)
{
    this->buttonCornderRadius = newRadius;
}

void CoolButtonLookAndFeel::setOffsetDeficit(int newOffsetX, int newOffsetY)
{
    this->offsetDeficitX = newOffsetX;
    this->offsetDeficitY = newOffsetY;
}
