#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NamJUCEAudioProcessorEditor::NamJUCEAudioProcessorEditor (NamJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), namEditor(p)
{

    setSize (950, 650);
    
    //Main NAM Editor
    addAndMakeVisible(&namEditor);
    namEditor.setBounds(getLocalBounds());

    //Resize Button
    resizeButton.reset(new juce::ImageButton("ResizeBtn"));
    addAndMakeVisible(resizeButton.get());
    resizeButton->toFront(false);

    hiddenResizeToggle.reset(new juce::ToggleButton("HiddenToggle"));
    resizeToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "SMALL_WINDOW_ID", *hiddenResizeToggle));

    resizeButton->onClick = [this]
    {
        hiddenResizeToggle->setToggleState(!hiddenResizeToggle->getToggleState(), true);
        setPluginSize(*audioProcessor.apvts.getRawParameterValue("SMALL_WINDOW_ID"));
    };

    //Check every time the GUI is reconstructed
    setPluginSize(*audioProcessor.apvts.getRawParameterValue("SMALL_WINDOW_ID"));    
    
}

NamJUCEAudioProcessorEditor::~NamJUCEAudioProcessorEditor()
{
    resizeToggleAttachment = nullptr;
}

//==============================================================================
void NamJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{

}

void NamJUCEAudioProcessorEditor::resized()
{
    
}

void NamJUCEAudioProcessorEditor::setPluginSize(bool makeSmall)
{
    if(makeSmall)
    {
        namEditor.setScaleFactor(0.8);
        setSize (int(950 * 0.8), int(650 * 0.8));
        resizeButton->setBounds(getWidth() - 40, getHeight() - 70, 30, 30);
        resizeButton->setImages(false, true, false, arrowExpand, 0.35f, juce::Colours::transparentWhite, arrowExpand, 0.6f, juce::Colours::transparentWhite, arrowExpand, 1.0, juce::Colours::transparentWhite, 0);
    }
    else
    {
        namEditor.setScaleFactor(1.0);
        setSize (950, 650);
        resizeButton->setBounds(getWidth() - 40, getHeight() - 75, 30, 30);
        resizeButton->setImages(false, true, false, arrowContract, 0.35f, juce::Colours::transparentWhite, arrowContract, 0.6f, juce::Colours::transparentWhite, arrowContract, 1.0, juce::Colours::transparentWhite, 0);
    }
}

void NamJUCEAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{   
    
}

void NamJUCEAudioProcessorEditor::timerCallback()
{

}