#ifndef __NAM_METER_LNF_H__
#define __NAM_METER_LNF_H__

#include <ff_meters/ff_meters.h>

using namespace juce;

class MeterLookAndFeel : public foleys::LevelMeterLookAndFeel
{
    juce::Rectangle<float> drawBackground(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds)
    {
        g.setColour(juce::Colours::transparentWhite);
        if (meterType & foleys::LevelMeter::HasBorder)
        {
            const auto corner = std::min(bounds.getWidth(), bounds.getHeight()) * 0.01f;
            g.fillRoundedRectangle(bounds, corner);
            g.setColour(findColour(foleys::LevelMeter::lmOutlineColour));
            g.drawRoundedRectangle(bounds.reduced(3), corner, 2);
            return bounds.reduced(3 + corner);
        }
        else
        {
            g.fillRect(bounds);
            return bounds;
        }
    }

    void drawMeterBars(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds,
                       const foleys::LevelMeterSource* source, int fixedNumChannels = -1, int selectedChannel = -1)
    {
        if (source == nullptr)
            return;

        const juce::Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
        const int numChannels = source->getNumChannels();
        if (meterType & foleys::LevelMeter::Minimal)
        {
            if (meterType & foleys::LevelMeter::Horizontal)
            {
                const float height = innerBounds.getHeight() / (2 * numChannels - 1);
                juce::Rectangle<float> meter = innerBounds.withHeight(height);
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    meter.setY(height * channel * 2);
                    {
                        juce::Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
                        drawMeterBar(g, meterType, meterBarBounds, source->getRMSLevel(channel), source->getMaxLevel(channel));
                        const float reduction = source->getReductionLevel(channel);
                        if (reduction < 1.0)
                            drawMeterReduction(g, meterType, meterBarBounds.withBottom(meterBarBounds.getCentreY()), reduction);
                    }

                    juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
                    if (!clip.isEmpty())
                        drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
                    juce::Rectangle<float> maxNum = getMeterMaxNumberBounds(meter, meterType);

                    if (!maxNum.isEmpty())
                        drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));

                    if (channel < numChannels - 1)
                    {
                        meter.setY(height * (channel * 2 + 1));
                        juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
                        if (!ticks.isEmpty())
                            drawTickMarks(g, meterType, ticks);
                    }
                }
            }
            else
            {
                const float width = innerBounds.getWidth() / (2 * numChannels - 1);
                juce::Rectangle<float> meter = innerBounds.withWidth(width);
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    meter.setX(width * channel * 2);
                    {
                        juce::Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
                        drawMeterBar(g, meterType, getMeterBarBounds(meter, meterType), source->getRMSLevel(channel), source->getMaxLevel(channel));
                        const float reduction = source->getReductionLevel(channel);
                        if (reduction < 1.0)
                            drawMeterReduction(g, meterType, meterBarBounds.withLeft(meterBarBounds.getCentreX()), reduction);
                    }
                    juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
                    if (!clip.isEmpty())
                        drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
                    juce::Rectangle<float> maxNum =
                        getMeterMaxNumberBounds(innerBounds.withWidth(innerBounds.getWidth() / numChannels)
                                                    .withX(innerBounds.getX() + channel * (innerBounds.getWidth() / numChannels)),
                                                meterType);
                    if (!maxNum.isEmpty())
                        drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));
                    if (channel < numChannels - 1)
                    {
                        meter.setX(width * (channel * 2 + 1));
                        juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
                        if (!ticks.isEmpty())
                            drawTickMarks(g, meterType, ticks);
                    }
                }
            }
        }
        else if (meterType & foleys::LevelMeter::SingleChannel)
        {
            if (selectedChannel >= 0)
                drawMeterChannel(g, meterType, innerBounds, source, selectedChannel);
        }
        else
        {
            const int numDrawnChannels = fixedNumChannels < 0 ? numChannels : fixedNumChannels;
            for (int channel = 0; channel < numChannels; ++channel)
                drawMeterChannel(g, meterType, getMeterBounds(innerBounds, meterType, numDrawnChannels, channel), source, channel);
        }
    }

    void drawMeterChannel(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds,
                          const foleys::LevelMeterSource* source, int selectedChannel)
    {
        if (source == nullptr)
            return;

        juce::Rectangle<float> meter = getMeterBarBounds(bounds, meterType);
        if (!meter.isEmpty())
        {
            if (meterType & foleys::LevelMeter::Reduction)
            {
                drawMeterBar(g, meterType, meter, source->getReductionLevel(selectedChannel), 0.0f);
            }
            else
            {
                drawMeterBar(g, meterType, meter, source->getRMSLevel(selectedChannel), source->getMaxLevel(selectedChannel));
                const float reduction = source->getReductionLevel(selectedChannel);
                if (reduction < 1.0)
                {
                    if (meterType & foleys::LevelMeter::Horizontal)
                        drawMeterReduction(g, meterType, meter.withBottom(meter.getCentreY()), reduction);
                    else
                        drawMeterReduction(g, meterType, meter.withLeft(meter.getCentreX()), reduction);
                }
            }
        }

        if (source->getClipFlag(selectedChannel))
        {
            juce::Rectangle<float> clip = getMeterClipIndicatorBounds(bounds, meterType);
            if (!clip.isEmpty())
                drawClipIndicator(g, meterType, clip, true);
        }

        juce::Rectangle<float> maxes = getMeterMaxNumberBounds(bounds, meterType);
        if (!maxes.isEmpty())
        {
            if (meterType & foleys::LevelMeter::Reduction)
                drawMaxNumber(g, meterType, maxes, source->getReductionLevel(selectedChannel));
            else
                drawMaxNumber(g, meterType, maxes, source->getMaxOverallLevel(selectedChannel));
        }
    }

    void drawTickMarks(juce::Graphics&, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {}

    void drawMaxNumber(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds, float maxGain) override {}

    void drawMaxNumberBackground(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {}

    void drawClipIndicatorBackground(juce::Graphics& g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {}
};

#endif // __NAM_METER_LNF_H__
