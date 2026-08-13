#include "EqEditor.h"

EqEditor::EqEditor(NamJUCEAudioProcessor& p, bool drawFade) : AudioProcessorEditor(&p), audioProcessor(p)
{
    sliderLnf.setColour(juce::BubbleComponent::backgroundColourId, audioProcessor.getPreferences().popupMenuColour);
    sliderLnf.setColour(juce::BubbleComponent::ColourIds::outlineColourId, juce::Colours::snow.withAlpha(0.25f));
        
    addAndMakeVisible(&inGainSlider);
    inGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    inGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    inGainSlider.setLookAndFeel(&lnf);
    inGainSlider.setAlpha(0.8);

    inLevelLabel.setText("INPUT\nLEVEL", juce::dontSendNotification);
    inLevelLabel.setJustificationType(juce::Justification::centred);
    inLevelLabel.attachToComponent(&inGainSlider, false);
    inLevelLabel.setFont(11.0f);
    inLevelLabel.setAlpha(0.55f);

    addAndMakeVisible(&outGainSlider);
    outGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outGainSlider.setLookAndFeel(&lnf);
    outGainSlider.setAlpha(0.8);
    outLevelLabel.setText("OUTPUT\nLEVEL", juce::dontSendNotification);
    outLevelLabel.setJustificationType(juce::Justification::centred);
    outLevelLabel.attachToComponent(&outGainSlider, false);
    outLevelLabel.setFont(11.0f);
    outLevelLabel.setAlpha(0.55f);

    placeSliders();

    addAndMakeVisible(&bypass);
    bypass.setAlpha(0.0);
    bypass.setBounds(145, 315, 35, 35);
    // bypass.onClick = [this] { updateGraphics(); }; // Parent value listener takes care of this.

    bypassButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "EQ_BYPASS_STATE_ID", bypass));

    // Slider apvts attachments
    for (int band = 0; band <= 9; ++band)
        sliderAttachments[band] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, "TEN_BAND_" + std::to_string(band + 1) + "_ID", sliders[band]);

    inputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "EQ_INPUT_GAIN_ID", inGainSlider);
    outputGainAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "EQ_OUTPUT_GAIN_ID", outGainSlider);
    

    if (drawFade)
    {
        fadeComponent.reset(new EqFadeComponent());
        addAndMakeVisible(fadeComponent.get());
    }

    addAndMakeVisible(&ledComp);
    ledComp.setLedOn(*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));
    ledComp.setBounds(bypass.getX() + 2, bypass.getY() - 42, 36, 36);

    if (drawFade)
        ledComp.setAlpha(0.2f);
}

EqEditor::~EqEditor()
{
    bypassButtonAttachment = nullptr;
    fadeComponent = nullptr;

    for (int band = 0; band <= 9; ++band)
        sliderAttachments[band] = nullptr;

    inputGainAttachment = nullptr;
    outputGainAttachment = nullptr;

    // audioProcessor.eqModuleVisible = false;
}

void EqEditor::paint(juce::Graphics& g)
{
    g.drawImageAt(background, 0, 0);

    g.drawImageAt(*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID") ? toggleOnImage 
            : toggleOffImage, bypass.getX() - 5, bypass.getY());
}

void EqEditor::resized()
{
    getLocalBounds();
    int knobSize = 70;
    inGainSlider.setBounds(70, 50, knobSize, knobSize);
    outGainSlider.setBounds(getWidth() - 140, 50, knobSize, knobSize);
    
    if(fadeComponent != nullptr)
        fadeComponent->setBounds(getLocalBounds());
}

void EqEditor::placeSliders()
{
    sliderLnf.setColour(CoolButtons::Slider::ColourIds::thumbGlowColourId,
            audioProcessor.getPreferences().getColourSchemeColour(ColourScheme::ColoursIds::eqLedColourId));

    int xSlider = 185;
    for (int i = 0; i <= 9; ++i)
    {
        addAndMakeVisible(&sliders[i]);
        sliders[i].setSliderStyle(Slider::SliderStyle::LinearVertical);
        sliders[i].setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        sliders[i].setPopupDisplayEnabled(true, true, getTopLevelComponent());
        sliders[i].setTextValueSuffix(" dB");
        sliders[i].setCustomSlider(CustomSlider::SliderTypes::EQ_Slider);

        sliders[i].setLookAndFeel(&sliderLnf);

        sliders[i].setBounds(xSlider, 235 + globalOffset, 40, 171);


        xSlider = xSlider + 60;
    }

    sliderLnf.setGlowEnabled(*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));
}

void EqEditor::updateGraphics()
{
    sliderLnf.setGlowEnabled(*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));

    sliderLnf.setColour(CoolButtons::Slider::ColourIds::thumbGlowColourId,
            audioProcessor.getPreferences().getColourSchemeColour(ColourScheme::ColoursIds::eqLedColourId));

    ledComp.setLedOn(*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));

    // this->repaint();
}

void EqEditor::scaleFactorChanged(int parentWidth, int parentHeight)
{
    int x = 0;
    int y = 0;

    if (parentWidth < 950)
    {
        x = 1;
        y = 1;
    }
    else
    {
        x = 0;
        y = 0;
    }

    sliderLnf.setOffsetDeficit(x, y);
}
