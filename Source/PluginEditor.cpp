#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NamJUCEAudioProcessorEditor::NamJUCEAudioProcessorEditor (NamJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize (560, 400);

    int knobOffset = 85;
    int knobSize = 76;
    int xStart = 15;
    int xOffsetMultiplier = 90;

    //Setup sliders
    for(int slider = 0; slider <= 5; ++slider)
    {
        sliders[slider].reset(new juce::Slider("slider" + std::to_string(slider)));
        addAndMakeVisible(sliders[slider].get());
        //sliders[slider]->setLookAndFeel(&lnf);
        sliders[slider]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[slider]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        sliders[slider]->setPopupDisplayEnabled(true, true, getTopLevelComponent());

        sliders[slider]->setBounds(xStart + (slider * xOffsetMultiplier), 65 + knobOffset, knobSize, knobSize);
    }

    sliderAttachments[PluginKnobs::Input] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT_ID", *sliders[PluginKnobs::Input]);
    sliderAttachments[PluginKnobs::NoiseGate] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NGATE_ID", *sliders[PluginKnobs::NoiseGate]);
    sliderAttachments[PluginKnobs::Bass] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "BASS_ID", *sliders[PluginKnobs::Bass]);
    sliderAttachments[PluginKnobs::Middle] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIDDLE_ID", *sliders[PluginKnobs::Middle]);
    sliderAttachments[PluginKnobs::Treble] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "TREBLE_ID", *sliders[PluginKnobs::Treble]);
    sliderAttachments[PluginKnobs::Output] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTPUT_ID", *sliders[PluginKnobs::Output]);
    
}

NamJUCEAudioProcessorEditor::~NamJUCEAudioProcessorEditor()
{
    for(int sliderAtt = 0; sliderAtt <= 5; ++sliderAtt)
        sliderAttachments[sliderAtt] = nullptr;
}

//==============================================================================
void NamJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    g.drawFittedText("Input", sliders[PluginKnobs::Input]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Noise Gate", sliders[PluginKnobs::NoiseGate]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Bass", sliders[PluginKnobs::Bass]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Middle", sliders[PluginKnobs::Middle]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Treble", sliders[PluginKnobs::Treble]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Output", sliders[PluginKnobs::Output]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
}

void NamJUCEAudioProcessorEditor::resized()
{
    
}
