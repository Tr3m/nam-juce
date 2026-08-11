#ifndef __NAM_SLIDER_LNF_H__
#define __NAM_SLIDER_LNF_H__

using namespace juce;

class SliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SliderLookAndFeel(int status, int orientation)
    {
        if (orientation == Orientation::Vertical)
        {
            if (status == Status::ON)
                thumbImage = ImageFileFormat::loadFrom(BinaryData::eq_thumb_on_png, BinaryData::eq_thumb_on_pngSize);
            else
                thumbImage = ImageFileFormat::loadFrom(BinaryData::eq_thumb_off_png, BinaryData::eq_thumb_off_pngSize);
        }
        else
        {
            if (status == Status::ON)
                thumbImage = ImageFileFormat::loadFrom(BinaryData::slim_thumb_on_png, BinaryData::slim_thumb_on_pngSize);
            else
                thumbImage = ImageFileFormat::loadFrom(BinaryData::slim_thumb_off_png, BinaryData::slim_thumb_off_pngSize);
        }
    }

    int getSliderThumbRadius (Slider&) override
    {
        return 19;
    }

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
                          const Slider::SliderStyle style, Slider& slider)
    {

        if (slider.isBar())
        {
            g.setColour(juce::Colours::transparentBlack);
            g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>(static_cast<float>(x), y + 0.5f, sliderPos - x, height - 1.0f)
                                             : juce::Rectangle<float>(x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
        }
        else
        {
            auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
            auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

            auto trackWidth = jmin(13.0f, slider.isHorizontal() ? height * 0.25f : width * 13.25f);


            Point<float> startPoint(slider.isHorizontal() ? x : x + width * 0.5f, slider.isHorizontal() ? y + height * 0.5f : height + y);

            Point<float> endPoint(slider.isHorizontal() ? width + x : startPoint.x, slider.isHorizontal() ? startPoint.y : y);

            Path backgroundTrack;
            backgroundTrack.startNewSubPath(startPoint);
            backgroundTrack.lineTo(endPoint);
            g.setColour(juce::Colours::transparentBlack);
            g.strokePath(backgroundTrack, {trackWidth, PathStrokeType::beveled, PathStrokeType::rounded});

            Path valueTrack;
            Point<float> minPoint, maxPoint, thumbPoint;

            if (isTwoVal || isThreeVal)
            {
                minPoint = {slider.isHorizontal() ? minSliderPos : width * 0.5f, slider.isHorizontal() ? height * 0.5f : minSliderPos};

                if (isThreeVal)
                    thumbPoint = {slider.isHorizontal() ? sliderPos : width * 0.5f, slider.isHorizontal() ? height * 0.5f : sliderPos};

                maxPoint = {slider.isHorizontal() ? maxSliderPos : width * 0.5f, slider.isHorizontal() ? height * 0.5f : maxSliderPos};
            }
            else
            {
                auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
                auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

                minPoint = startPoint;
                maxPoint = {kx, ky};
            }

            auto thumbWidth = getSliderThumbRadius(slider);

            valueTrack.startNewSubPath(minPoint);
            valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
            g.setColour(juce::Colours::transparentBlack);
            g.strokePath(valueTrack, {trackWidth, PathStrokeType::curved, PathStrokeType::rounded});
            g.setColour(juce::Colours::black);

            if (!isTwoVal)
            {
                auto centre = isThreeVal ? thumbPoint : maxPoint;

                g.drawImage(thumbImage,
                            juce::Rectangle<float>(
                                (float)thumbImage.getWidth(),
                                (float)thumbImage.getHeight())
                                .withCentre(centre),
                            false);

                // g.drawImage(thumbImage,
                //             juce::Rectangle<float>(static_cast<float>(thumbWidth * 4), static_cast<float>(thumbWidth * 4))
                //                 .withCentre(isThreeVal ? thumbPoint : maxPoint),
                //             false);
            }

            if (isTwoVal || isThreeVal)
            {
                auto sr = jmin(trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
                auto pointerColour = slider.findColour(Slider::thumbColourId);

                if (slider.isHorizontal())
                {
                    drawPointer(g, minSliderPos - sr, jmax(0.0f, y + height * 0.5f - trackWidth * 2.0f), trackWidth * 2.0f, pointerColour, 2);

                    drawPointer(
                        g, maxSliderPos - trackWidth, jmin(y + height - trackWidth * 2.0f, y + height * 0.5f), trackWidth * 2.0f, pointerColour, 4);
                }
                else
                {
                    drawPointer(g, jmax(0.0f, x + width * 0.5f - trackWidth * 2.0f), minSliderPos - trackWidth, trackWidth * 2.0f, pointerColour, 1);

                    drawPointer(g, jmin(x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr, trackWidth * 2.0f, pointerColour, 3);
                }
            }
        }
    }
    
    /*
    void drawBubble (juce::Graphics& g, juce::BubbleComponent&, const juce::Point<float>&,
        const juce::Rectangle<float>& body) override
    {
        g.setColour (juce::Colours::black.withAlpha(0.95f));
        g.fillRoundedRectangle (body, 6.0f);

        g.setColour (juce::Colours::orange.withAlpha(0.65f));
        g.drawRoundedRectangle (body, 6.0f, 2.0f);
    }
    */


    enum Orientation
    {
        Horizontal = 0,
        Vertical
    };

    enum Status
    {
        ON = 0,
        OFF
    };


private:
    Image thumbImage;
};

#endif //__NAM_SLIDER_LNF_H__
