#include "EqEditor.h"

EqEditor::EqEditor(NamJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{  
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
    bypass.onClick = [this]
    {
        if (*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))        
            for (int i = 0; i <= 9; ++i)
                sliders[i].setLookAndFeel(&lnfOn);        
        else           
            for (int i = 0; i <= 9; ++i)
                sliders[i].setLookAndFeel(&lnfOff);
        
        repaint();
    };  

    bypassButtonAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "EQ_BYPASS_STATE_ID", bypass));

    if (*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))
    {
        for (int i = 0; i <= 9; ++i)
                sliders[i].setLookAndFeel(&lnfOn); 
    }
    else
    {
        for (int i = 0; i <= 9; ++i)
            sliders[i].setLookAndFeel(&lnfOff);
    }

    //Slider apvts attachments
    for(int band = 0; band <= 9; ++band)
      sliderAttachments[band] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, 
        "TEN_BAND_" + std::to_string(band + 1) + "_ID", sliders[band]);

    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "EQ_INPUT_GAIN_ID", inGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "EQ_OUTPUT_GAIN_ID", outGainSlider);
}

EqEditor::~EqEditor()
{
    bypassButtonAttachment = nullptr;

    for(int band = 0; band <= 9; ++band)
      sliderAttachments[band] = nullptr;

    inputGainAttachment = nullptr;
    outputGainAttachment = nullptr;
}

void EqEditor::paint(juce::Graphics& g)
{
  if (*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))
      g.drawImageAt(backgroundOn, 0, 0);
  else
      g.drawImageAt(backgroundOff, 0, 0);  
}

void EqEditor::resized()
{
    getLocalBounds();
    int knobSize = 70;
    inGainSlider.setBounds(70, 50, knobSize, knobSize);
    outGainSlider.setBounds(getWidth() - 140, 50, knobSize, knobSize);
}

void EqEditor::placeSliders()
{
  int xSlider = 190;
  for (int i = 0; i <= 9; ++i)
  {
    addAndMakeVisible(&sliders[i]);
    sliders[i].setSliderStyle(Slider::SliderStyle::LinearVertical);

    if (*audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))
        sliders[i].setLookAndFeel(&lnfOff);
    else
        sliders[i].setLookAndFeel(&lnfOn);

    sliders[i].setBounds(xSlider, 235 + globalOffset, 30, 144);

   
    xSlider = xSlider + 60;
    
  }
}