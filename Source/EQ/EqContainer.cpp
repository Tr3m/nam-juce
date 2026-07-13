#include "EqContainer.h"

EqContainer::EqContainer(NamJUCEAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), eqEditor(p), dummyEditor(p, true)
{
    addAndMakeVisible(&dummyEditor);
    addAndMakeVisible(&eqEditor);

    dummyEditor.setEnabled(false);
    dummyEditor.setAlpha(0.78f);
    
    audioProcessor.getEqStateValue().addListener(this);
}

EqContainer::~EqContainer()
{
    audioProcessor.getEqStateValue().removeListener(this);
}

void EqContainer::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void EqContainer::resized ()
{
    eqEditor.setBounds(getLocalBounds());
    dummyEditor.setBounds(0, eqEditor.getX() - 205, getWidth(), getHeight());
    dummyEditor.setTransform(juce::AffineTransform::verticalFlip(dummyEditor.getHeight()));
}


void EqContainer::toggleEq()
{
    eqEditor.toggleEq();
}

void EqContainer::updateGraphics()
{
    eqEditor.updateGraphics();
    dummyEditor.updateGraphics();
}


void EqContainer::valueChanged (Value& value )
{
    if (value == audioProcessor.getEqStateValue())
        updateGraphics();
}
