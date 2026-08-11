#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/LookAndFeel.h"

using namespace juce;

class LeduttonLNF : public juce::LookAndFeel_V4
{
public:
    LeduttonLNF() {

    };

    void drawImageButton(Graphics& g, Image* image, Image* labelImage, int imageX, int imageY, int imageW, int imageH, const Colour& overlayColour,
                         float imageOpacity, ImageButton& button)
    {
        if (!button.isEnabled())
            imageOpacity *= 0.3f;

        AffineTransform t = RectanglePlacement(RectanglePlacement::stretchToFit)
                                .getTransformToFit(image->getBounds().toFloat(), juce::Rectangle<int>(imageX, imageY, imageW, imageH).toFloat());

        if (!overlayColour.isOpaque())
        {
            g.setOpacity(imageOpacity);
            g.drawImageTransformed(*image, t, false);
        }

        if (!overlayColour.isTransparent())
        {
            g.setColour(overlayColour);
            g.drawImageTransformed(*image, t, true);
        }

        g.drawImageTransformed(*labelImage, t, false);
    }
};

class LedButtonComponent : public juce::ImageButton
{
public:
    LedButtonComponent(const juce::Image state_on, const juce::Image state_off) : ImageButton()
    {
        stateOffLabel = state_off;
        stateOnLabel = state_on;
        stateLabel = state_off;

        setLookAndFeel(&lnf);
        this->setImages(false, true, false, stateUp, 1.0, juce::Colours::transparentWhite, stateUp, 1.0, juce::Colours::transparentWhite, stateDown,
                        1.0, juce::Colours::transparentWhite, 0);
    };

    void setStateLabelImages(const juce::Image state_on, const juce::Image state_off)
    {
        stateOffLabel = state_off;
        stateOnLabel = state_on;
    };

    void setLedState(bool isLedOn)
    {
        ledState = isLedOn;
        stateLabel = isLedOn ? stateOnLabel : stateOffLabel;
        this->repaint();
    };

    bool getLedState() { return ledState; };


    void setImages(const bool resizeButtonNowToFitThisImage, const bool rescaleImagesWhenButtonSizeChanges, const bool preserveImageProportions,
                   const Image& normalImage_, const float imageOpacityWhenNormal, Colour overlayColourWhenNormal, const Image& overImage_,
                   const float imageOpacityWhenOver, Colour overlayColourWhenOver, const Image& downImage_, const float imageOpacityWhenDown,
                   Colour overlayColourWhenDown, const float hitTestAlphaThreshold)
    {
        normalImage = normalImage_;
        overImage = overImage_;
        downImage = downImage_;

        if (resizeButtonNowToFitThisImage && normalImage.isValid())
        {
            imageBounds.setSize(normalImage.getWidth(), normalImage.getHeight());

            setSize(imageBounds.getWidth(), imageBounds.getHeight());
        }

        scaleImageToFit = rescaleImagesWhenButtonSizeChanges;
        preserveProportions = preserveImageProportions;

        normalOpacity = imageOpacityWhenNormal;
        normalOverlay = overlayColourWhenNormal;
        overOpacity = imageOpacityWhenOver;
        overOverlay = overlayColourWhenOver;
        downOpacity = imageOpacityWhenDown;
        downOverlay = overlayColourWhenDown;

        alphaThreshold = (uint8)jlimit(0, 0xff, roundToInt(255.0f * hitTestAlphaThreshold));

        this->repaint();
    };

    void setLabelVisible(bool label_visible)
    {
        labelVisible = label_visible;
        this->repaint();
    };

    void reloadImages()
    {
        this->setImages(false, true, false, stateUp, 1.0, juce::Colours::transparentWhite, stateUp, 1.0, juce::Colours::transparentWhite, stateDown,
                        1.0, juce::Colours::transparentWhite, 0);
        this->repaint();
    };

private:
    Image getCurrentImage() const
    {
        if (isDown() || getToggleState())
            return getDownImage();

        if (isOver())
            return getOverImage();

        return getNormalImage();
    }

    void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        if (!isEnabled())
        {
            shouldDrawButtonAsHighlighted = false;
            shouldDrawButtonAsDown = false;
        }

        Image im(getCurrentImage());

        if (im.isValid())
        {
            const int iw = im.getWidth();
            const int ih = im.getHeight();
            int w = getWidth();
            int h = getHeight();
            int x = (w - iw) / 2;
            int y = (h - ih) / 2;

            if (scaleImageToFit)
            {
                if (preserveProportions)
                {
                    int newW, newH;
                    const float imRatio = (float)ih / (float)iw;
                    const float destRatio = (float)h / (float)w;

                    if (imRatio > destRatio)
                    {
                        newW = roundToInt((float)h / imRatio);
                        newH = h;
                    }
                    else
                    {
                        newW = w;
                        newH = roundToInt((float)w * imRatio);
                    }

                    x = (w - newW) / 2;
                    y = (h - newH) / 2;
                    w = newW;
                    h = newH;
                }
                else
                {
                    x = 0;
                    y = 0;
                }
            }

            if (!scaleImageToFit)
            {
                w = iw;
                h = ih;
            }

            imageBounds.setBounds(x, y, w, h);

            const bool useDownImage = shouldDrawButtonAsDown || getToggleState();

            if (labelVisible)
            {
                lnf.drawImageButton(g, &im, &stateLabel, x, y, w, h,
                                    useDownImage ? downOverlay : (shouldDrawButtonAsHighlighted ? overOverlay : normalOverlay),
                                    useDownImage ? downOpacity : (shouldDrawButtonAsHighlighted ? overOpacity : normalOpacity), *this);
            }
            else
            {
                lnfPlain.drawImageButton(g, &im, x, y, w, h,
                                         useDownImage ? downOverlay : (shouldDrawButtonAsHighlighted ? overOverlay : normalOverlay),
                                         useDownImage ? downOpacity : (shouldDrawButtonAsHighlighted ? overOpacity : normalOpacity), *this);
            }
        }
    }

    Image getNormalImage() const { return normalImage; }

    Image getOverImage() const { return overImage.isValid() ? overImage : normalImage; }

    Image getDownImage() const { return downImage.isValid() ? downImage : getOverImage(); }

    bool hitTest(int x, int y)
    {
        if (!Component::hitTest(x, y)) // handle setInterceptsMouseClicks
            return false;

        if (alphaThreshold == 0)
            return true;

        Image im(getCurrentImage());

        return im.isNull()
               || ((!imageBounds.isEmpty())
                   && alphaThreshold < im.getPixelAt(((x - imageBounds.getX()) * im.getWidth()) / imageBounds.getWidth(),
                                                     ((y - imageBounds.getY()) * im.getHeight()) / imageBounds.getHeight())
                                           .getAlpha());
    }

private:
    bool scaleImageToFit, preserveProportions;
    Colour normalOverlay, overOverlay, downOverlay;
    float normalOpacity, overOpacity, downOpacity;
    Image normalImage, overImage, downImage;
    uint8 alphaThreshold;
    juce::Rectangle<int> imageBounds;
    juce::Image stateDown = juce::ImageFileFormat::loadFrom(BinaryData::buttonpushed_png, BinaryData::buttonpushed_pngSize);
    juce::Image stateUp = juce::ImageFileFormat::loadFrom(BinaryData::buttonunpushed_png, BinaryData::buttonunpushed_pngSize);
    juce::Image stateLabel, stateOffLabel, stateOnLabel;
    // juce::ImageFileFormat::loadFrom(BinaryData::stateoff_png, BinaryData::stateoff_pngSize);

    bool ledState{false};
    bool labelVisible{true};

    LeduttonLNF lnf;
    LookAndFeel_V2 lnfPlain;
};
