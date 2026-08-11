#include "EqContainer.h"

EqContainer::EqContainer(NamJUCEAudioProcessor& p, std::unique_ptr<EqContainer>& selfRef)
    : AudioProcessorEditor(&p), audioProcessor(p), eqEditor(p), dummyEditor(p, true), self(selfRef)
{
    meterlnf.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::ivory);

    meterIn.setLookAndFeel(&meterlnf);
    meterOut.setLookAndFeel(&meterlnf);

    // Meters
    meterIn.setMeterSource(&audioProcessor.getMeterInSource());
    addAndMakeVisible(meterIn);

    meterOut.setMeterSource(&audioProcessor.getMeterOutSource());
    addAndMakeVisible(meterOut);

    meterIn.setAlpha(0.8);
    meterOut.setAlpha(0.8);

    meterIn.setSelectedChannel(0);
    meterOut.setSelectedChannel(0);
    addAndMakeVisible(&dummyEditor);
    addAndMakeVisible(&eqEditor);

    dummyEditor.setEnabled(false);
    dummyEditor.setAlpha(0.6f);

    addAndMakeVisible(&closeButton);
    closeButton.setImages(false, true, false, xIcon, 0.7f, juce::Colours::transparentWhite, xIcon, 1.0f,
            juce::Colours::transparentWhite, xIcon, 0.65f, juce::Colours::transparentWhite, 0.0f);

    closeButton.onClick = [this] 
    {
        DBG("Goodbye EQ Container!");
        audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::EQ_BYPASS)->removeListener(this);
        audioProcessor.eqModuleVisible = false;
        self.reset();
    };
    
    audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::EQ_BYPASS)->addListener(this);
}

EqContainer::~EqContainer()
{
    DBG("Goodbye EQ Container! (Destructor)");
    audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::EQ_BYPASS)->removeListener(this);
    self.reset();
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

    closeButton.setBounds(getWidth() - 43, 25, 20, 20);

    int meterHeight = 255;
    int meterWidth = 20;
    meterIn.setBounds(20, (getHeight() / 2) - (meterHeight / 2) + 10, meterWidth, meterHeight);
    meterOut.setBounds(getWidth() - 30, (getHeight() / 2) - (meterHeight / 2) + 10, meterWidth, meterHeight);
}

void EqContainer::updateGraphics()
{
    eqEditor.updateGraphics();
    dummyEditor.updateGraphics();
}


void EqContainer::valueChanged (Value& value )
{
    if (value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::EQ_BYPASS)))
        updateGraphics();
}
