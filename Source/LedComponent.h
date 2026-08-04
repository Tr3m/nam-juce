#ifndef __LED_COMPONENT_H__
#define __LED_COMPONENT_H__

#include <JuceHeader.h>

class LedComponent : public juce::Component
{
public:
    LedComponent()
    {
        // this->setOpaque(true);
    };

    ~LedComponent() {};
    
    void paint(juce::Graphics& g) override
    {
        g.drawImageAt(led_to_draw, 0, 0);
    };

    void resized() override {};

    void setLedOn(bool shouldBeOn)
    {
        this->led_to_draw = shouldBeOn ? led_on : led_off;
        this->repaint();
    };
        

private:
    juce::Image led_off = juce::ImageFileFormat::loadFrom(BinaryData::led_off_png, BinaryData::led_off_pngSize);
    juce::Image led_on = juce::ImageFileFormat::loadFrom(BinaryData::led_on_png, BinaryData::led_on_pngSize);
    juce::Image led_to_draw {led_on};

};

#endif // __LED_COMPONENT_H__
