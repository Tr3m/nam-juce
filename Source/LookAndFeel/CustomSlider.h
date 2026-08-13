#ifndef __CUSTOM_SLIDER_H__
#define __CUSTOM_SLIDER_H__

class CustomSlider : public juce::Slider
{
public:
    CustomSlider(int sliderIndex = 0) { this->sliderIndex = sliderIndex; };

    void setCustomSlider(int sliderIndex) { this->sliderIndex = sliderIndex; };

    String getTextFromValue(double value) override
    {
        switch (sliderIndex)
        {
            case SliderTypes::Doubler:
                if (value == 0)
                    return "OFF";
                else
                    return juce::String(value) + " ms";
                break;
            case SliderTypes::Gate:
                if (value == -101)
                    return "OFF";
                else
                    return juce::String(value) + " dB";
                break;
            case SliderTypes::Filters:
                if (value <= 20 || value >= 20000)
                    return "OFF";
                else
                    return juce::String(value) + " Hz";
                break;

            case SliderTypes::EQ_Slider:
                if (value == 0)
                    return juce::String(static_cast<int>(value)) + this->getTextValueSuffix();
                else if (value > 0)
                    return "+" + juce::String(std::round(value * 10.0) / 10.0) + this->getTextValueSuffix();
                else 
                    return juce::String(std::round(value * 10.0) / 10.0) + this->getTextValueSuffix();
                break;
            case SliderTypes::Slim_Slider:
                return "Slim: " + String(value);
                break;
            default: return String(value) + this->getTextValueSuffix(); break;

        }
    };

    enum SliderTypes
    {
        Default = 0,
        Doubler,
        Gate,
        Filters,
        EQ_Slider,
        Slim_Slider
    };

private:
    int sliderIndex{0};
};

#endif //__CUSTOM_SLIDER_H__
