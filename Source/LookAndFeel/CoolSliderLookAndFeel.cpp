#include "CoolSliderLookAndFeel.h"

using namespace juce;

CoolSliderLookAndFeel::CoolSliderLookAndFeel()
{
    this->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::black);
    this->setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
}

CoolSliderLookAndFeel::CoolSliderLookAndFeel(int thumbStyle, bool glowEnabled)
{
    this->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::transparentBlack);
    this->setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
    this->setColour(juce::Slider::ColourIds::thumbColourId, this->thumbColour);
    this->glowEnabled = glowEnabled;

    if (thumbStyle < 0 || thumbStyle > 1)
        thumbStyle = 0;

    this->thumbStyle = thumbStyle;
}

CoolSliderLookAndFeel::CoolSliderLookAndFeel(int rotaryDrawMethod)
{
    this->setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::transparentBlack);
    this->setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
    this->setColour(juce::Slider::ColourIds::thumbColourId, this->thumbColour);

    if (rotaryDrawMethod < 0 || rotaryDrawMethod > 2)
        rotaryDrawMethod = 0;

    this->rotarySliderDrawMethod = rotaryDrawMethod;
}

void CoolSliderLookAndFeel::setColour (int colourId, juce::Colour colour)
{
    switch (colourId)
    {
        case (CoolButtons::Slider::ColourIds::thumbGlowColourId):
            this->glowColour = colour;
            break;
        case (CoolButtons::Slider::ColourIds::thumbLedOffColourId):
            this->thumbLedBackgroundColour = colour;
            break;
        case (CoolButtons::Slider::ColourIds::rotarySliderColourId):
            this->rotarySliderColour = colour; 
            break;
        default:
            LookAndFeel_V4::setColour(colourId, colour);
            break;
    }
}


juce::Colour CoolSliderLookAndFeel::findColour(int colourId)
{
    switch (colourId)
    {
        case CoolButtons::Slider::ColourIds::thumbGlowColourId:
            return this->glowColour;
            break;
        case (CoolButtons::Slider::ColourIds::thumbLedOffColourId):
            return this->thumbLedBackgroundColour;
            break;
        case (CoolButtons::Slider::ColourIds::rotarySliderColourId):
            return this->rotarySliderColour;
            break;
        default:
            return LookAndFeel_V4::findColour(colourId);
            break;
    }
}

void CoolSliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                            : Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)));

        drawLinearSliderOutline (g, x, y, width, height, style, slider);
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        // auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);
        auto trackWidth = slider.isHorizontal() ? (float) height * 0.3f : (float) width * 0.3f;

        Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                    slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                                slider.isHorizontal() ? startPoint.y : (float) y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
        

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float) width * 0.5f,
                            slider.isHorizontal() ? (float) height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float) width * 0.5f,
                                slider.isHorizontal() ? (float) height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float) width * 0.5f,
                            slider.isHorizontal() ? (float) height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = this->getSliderThumbRadius (slider);

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Rectangle<float> trackBounds;

        if (slider.isHorizontal())
        {
            trackBounds = Rectangle<float>(
                (float)x - 6.5f,
                (float)y + (float)height * 0.5f - trackWidth * 0.5f,
                (float)width + 12.5f,
                trackWidth + 2.0);
        }
        else
        {
            trackBounds = Rectangle<float>(
                (float)x + (float)width * 0.5f - trackWidth * 0.5f,
                (float)y - 6.5f,
                trackWidth,
                (float)height + 12.5f + 2.0);
        }
        
        // Track inner glow
        if (this->trackInnerGlowEnabled)
        {
            juce::Path innerShadowTrack;
            innerShadowTrack.addRoundedRectangle(trackBounds, trackWidth * 0.5f);
            innerTrackShadow.render(g, innerShadowTrack);
        }
        // g.setColour(juce::Colours::red.withAlpha(0.5f));
        // g.fillRoundedRectangle(trackBounds, trackWidth * 0.5f);


        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            // g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));

            Rectangle<float> rr(static_cast<float> ((slider.isHorizontal() ? 1.3 : 0.85) * thumbWidth), static_cast<float> ((slider.isHorizontal() ? 0.85 : 1.3) * thumbWidth));

            // Thumb Shadow
            Path thumbTrack;
            thumbTrack.addRoundedRectangle(rr.withCentre (isThreeVal ? thumbPoint : maxPoint), 10.0f);
            shadow.render(g, thumbTrack);

            g.fillRoundedRectangle(rr.withCentre (isThreeVal ? thumbPoint : maxPoint), 10.0f);
            // g.setColour(juce::Colours::grey); //Led Off colour
            g.setColour(thumbLedBackgroundColour); //Led Off colour
            
            // Thumb Inner Shadow 
            innerThumbShadowOffset = { 1 + offsetDeficitX, 1 + offsetDeficitY };
            innerThumbShadow = { innerThumbShadowColor, innerThumbShadowRadius, innerThumbShadowOffset, innerThumbShadowSpread };
            innerThumbShadow.render(g, thumbTrack);
            
            if (thumbStyle == ThumbStyles::withLed)
            {
                Rectangle<float> ledBounds(static_cast<float>(slider.isHorizontal() ? rr.getWidth() / 4 : rr.getWidth()), 
                            static_cast<float>(slider.isHorizontal() ? rr.getHeight() : rr.getHeight() / 4));

                // Led Glow
                if (glowEnabled)
                {
                    g.setColour(glowColour);
                    Path ledTrack;
                    ledTrack.addRectangle(ledBounds.withCentre (isThreeVal ? thumbPoint : maxPoint));
                    outerGlow = { glowColour.withAlpha(0.6f), glowRadius, glowOffset, glowSpread };
                    outerGlow.render(g, ledTrack);
                }

                // Led
                g.fillRect(ledBounds.withCentre (isThreeVal ? thumbPoint : maxPoint));
            }
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin (trackWidth, (slider.isHorizontal() ? (float) height : (float) width) * 0.4f);
            auto pointerColour = slider.findColour (Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                                jmax (0.0f, (float) y + (float) height * 0.5f - trackWidth * 2.0f),
                                trackWidth * 2.0f, pointerColour, 2);

                drawPointer (g, maxSliderPos - trackWidth,
                                jmin ((float) (y + height) - trackWidth * 2.0f, (float) y + (float) height * 0.5f),
                                trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, (float) x + (float) width * 0.5f - trackWidth * 2.0f),
                                minSliderPos - trackWidth,
                                trackWidth * 2.0f, pointerColour, 1);

                drawPointer (g, jmin ((float) (x + width) - trackWidth * 2.0f, (float) x + (float) width * 0.5f), maxSliderPos - sr,
                                trackWidth * 2.0f, pointerColour, 3);
            }
        }

        if (slider.isBar())
            drawLinearSliderOutline (g, x, y, width, height, style, slider);
    }
}

void CoolSliderLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    switch (this->rotarySliderDrawMethod)
    {
        case RotarySliderDrawMethods::paint:
            this->paintRotarySlider(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
            break;
        case RotarySliderDrawMethods::image:
            this->paintRotarySliderWithImage(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
            break;
        default:
            this->paintRotarySlider(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
            break;
    }
}

void CoolSliderLookAndFeel::paintRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    juce::Path valueTrack;
    valueTrack.addEllipse(bounds.toFloat());
    
    shadow.render(g, valueTrack);

    g.setColour(this->rotarySliderColour);
    g.fillEllipse(bounds.toFloat());

    innerThumbShadow.render(g, valueTrack);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (3.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    auto thumbWidth = lineW * 2.0f;
    auto thumbRadius = arcRadius - rotarySliderTumbOffset;
    
    Point<float> thumbPoint (
        bounds.getCentreX() + thumbRadius * std::cos (toAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + thumbRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (thumbLedBackgroundColour);

    if (glowEnabled)
    {
        g.setColour(glowColour);
        Path ledTrack;
        ledTrack.addEllipse(Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
        outerGlow = { glowColour, glowRadius, glowOffset, glowSpread };
        outerGlow.render(g, ledTrack);
    }

    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

void CoolSliderLookAndFeel::paintRotarySliderWithImage(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    // g.fillAll(juce::Colours::red.withAlpha(0.4f));

    auto bounds = juce::Rectangle<float>(x, y, width, height);
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
                0,
                knobImage.getWidth(),
                knobImage.getHeight());

    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const auto lineW = juce::jmin(3.0f, radius * 0.5f);
    const auto arcRadius = radius - lineW * 0.5f;

    const auto thumbWidth = lineW * 2.0f * rotarySliderThumbScale;
    const auto thumbRadius = arcRadius - rotarySliderTumbOffset;

    const juce::Point<float> thumbPoint(
        bounds.getCentreX() + thumbRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + thumbRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi)
    );

    const auto thumbBounds = juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint);

    g.setColour(thumbLedBackgroundColour);

    if (glowEnabled)
    {
        g.setColour(glowColour);

        juce::Path ledTrack;
        ledTrack.addEllipse(thumbBounds);

        outerGlow = { glowColour, glowRadius, glowOffset, glowSpread };

        outerGlow.render(g, ledTrack);
    }

    g.fillEllipse(thumbBounds);
}


int CoolSliderLookAndFeel::getSliderThumbRadius (juce::Slider& slider)
{
    return slider.isHorizontal() ? static_cast<int> ((float) slider.getHeight() * 0.5f)
                                 : static_cast<int> ((float) slider.getWidth()  * 0.5f);
}

void CoolSliderLookAndFeel::setThumbStyle(int style)
{
    if (style < 0 || style > 1)
        style = 0;

    this->thumbStyle = style;
}

void CoolSliderLookAndFeel::setGlowEnabled(bool shoudBeEnabled)
{
    this->glowEnabled = shoudBeEnabled;
}

void CoolSliderLookAndFeel::setInnerTrackGlowEnabled(bool trackInnerGlowEnabled)
{
    this->trackInnerGlowEnabled = trackInnerGlowEnabled;
}

void CoolSliderLookAndFeel::setRotarySliderDrawMethod(int method)
{
    switch (method)
    {
        case RotarySliderDrawMethods::paint:
            this->rotarySliderDrawMethod = RotarySliderDrawMethods::paint;
            break;
        case RotarySliderDrawMethods::image:
            this->rotarySliderDrawMethod = RotarySliderDrawMethods::image;
            break;
        default:
            this->rotarySliderDrawMethod = RotarySliderDrawMethods::paint;
            break;
    }
}

void CoolSliderLookAndFeel::setRotarySliderThumbOffset (float offset)
{
    this->rotarySliderTumbOffset = offset;
}

void CoolSliderLookAndFeel::setRotarySliderImage(const juce::Image& image)
{
    this->knobImage = image;
}

void CoolSliderLookAndFeel::setOffsetDeficit(int newOffsetX, int newOffsetY)
{
    this->offsetDeficitX = newOffsetX;
    this->offsetDeficitY = newOffsetY;
}

void CoolSliderLookAndFeel::setRotarySliderThumbScale (float newScale)
{
    this->rotarySliderThumbScale = newScale;
}
