#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NamJUCEAudioProcessorEditor::NamJUCEAudioProcessorEditor(NamJUCEAudioProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p), namEditor(p)
{
    addAndMakeVisible(&namEditor);
    namEditor.setBounds(0, 0, 950, 650);
  
    setResizable(true, true);
    double ratio = 19.0/13.0;

    setSize(audioProcessor.windowWidth, audioProcessor.windowWidth/ratio);
    // setSize(950.0,950.0/ratio);

    setResizeLimits(760, 760/ratio, 950, 950/ratio);
    getConstrainer()->setFixedAspectRatio(ratio);


}

NamJUCEAudioProcessorEditor::~NamJUCEAudioProcessorEditor()
{

}

//==============================================================================
void NamJUCEAudioProcessorEditor::paint(juce::Graphics& g) {}

void NamJUCEAudioProcessorEditor::resized()
{
    this->width = getWidth();
    this->height = getHeight();
    namEditor.setScaleFactor(calculateEditorScale(float(getWidth())));
    namEditor.scaleFactorChanged(width, height);
    
    audioProcessor.windowWidth = static_cast<float>(width);
    // DBG(static_cast<float>(width));
}

float NamJUCEAudioProcessorEditor::calculateEditorScale(int currentWidth)
{
    return 0.8 + ((1.0 - 0.8) / (950.0 - 760.0)) * (currentWidth - 760.0);
}
