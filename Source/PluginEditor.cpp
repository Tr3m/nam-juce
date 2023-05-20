#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NamJUCEAudioProcessorEditor::NamJUCEAudioProcessorEditor (NamJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize (560, 400);

    //Meters
    meterlnf.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::ivory);
    meterlnf.setColour(foleys::LevelMeter::lmMeterBackgroundColour, juce::Colours::transparentBlack);
    meterIn.setLookAndFeel(&meterlnf);
    meterIn.setMeterSource(&audioProcessor.getMeterInSource());
    addAndMakeVisible(meterIn);

    meterOut.setLookAndFeel(&meterlnf);
    meterOut.setMeterSource(&audioProcessor.getMeterOutSource());
    addAndMakeVisible(meterOut);

    meterIn.setAlpha(0.8);
    meterOut.setAlpha(0.8);

    meterIn.setSelectedChannel(0);
    meterOut.setSelectedChannel(0);  
	
    int knobSize = 76;
    int xStart = 15;
    int xOffsetMultiplier = 90;

    //Setup sliders
    for(int slider = 0; slider <= 5; ++slider)
    {
        sliders[slider].reset(new juce::Slider("slider" + std::to_string(slider)));
        addAndMakeVisible(sliders[slider].get());
        sliders[slider]->setLookAndFeel(&lnf);
        sliders[slider]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[slider]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        sliders[slider]->setPopupDisplayEnabled(true, true, getTopLevelComponent());

        sliders[slider]->setBounds(xStart + (slider * xOffsetMultiplier), 85, knobSize, knobSize);
    }

    initializeFilters();

    sliders[PluginKnobs::NoiseGate]->addListener(this);

    //Tone Stack Toggle
    toneStackToggle.reset(new juce::ToggleButton("ToneStackToggleButton"));
    addAndMakeVisible(toneStackToggle.get());
    toneStackToggle->setBounds(sliders[PluginKnobs::Bass]->getX() + 30, sliders[PluginKnobs::Bass]->getY() + knobSize - 9, 100, 40);
    toneStackToggle->setButtonText("Tone Stack");
    toneStackToggle->onClick = [this]{setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));};

    //Rerunning this for GUI Recustrunction upon reopning the plugin
    setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));

    //Normalize Toggle
    normalizeToggle.reset(new juce::ToggleButton("NormalizeToggleButton"));
    addAndMakeVisible(normalizeToggle.get());
    normalizeToggle->setBounds(toneStackToggle->getX() + 120, toneStackToggle->getY(), 100, 40);
    normalizeToggle->setButtonText("Normalize");

    //IR Toggle
    irToggle.reset(new juce::ToggleButton("IRToggleButton"));
    addAndMakeVisible(irToggle.get());
    irToggle->setBounds(normalizeToggle->getX() + 120, normalizeToggle->getY(), 100, 40);
    irToggle->setButtonText("IR");

    //Model Name Box and Button
    initializeTextBox("ModelNameBox", modelNameBox, 15, sliders[PluginKnobs::Input]->getY() + 160, 160, 24);
    
    initializeButton("LoadModelButton", loadModelButton, modelNameBox->getX() + modelNameBox->getWidth() + 5, modelNameBox->getY(), 40, modelNameBox->getHeight());
    loadModelButton->onClick = [this]{loadModelButtonClicked();};

    //IR Name Box and Button
    initializeTextBox("IRNameBox", irNameBox, 15, modelNameBox->getY() + 80, 160, 24);
    initializeButton("LoadIRButton", loadIRButton, irNameBox->getX() + irNameBox->getWidth() + 5, irNameBox->getY(), 40, irNameBox->getHeight());
    loadIRButton->onClick = [this]{loadIrButtonClicked();};

    sliderAttachments[PluginKnobs::Input] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUT_ID", *sliders[PluginKnobs::Input]);
    sliderAttachments[PluginKnobs::NoiseGate] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NGATE_ID", *sliders[PluginKnobs::NoiseGate]);
    sliderAttachments[PluginKnobs::Bass] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "BASS_ID", *sliders[PluginKnobs::Bass]);
    sliderAttachments[PluginKnobs::Middle] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIDDLE_ID", *sliders[PluginKnobs::Middle]);
    sliderAttachments[PluginKnobs::Treble] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "TREBLE_ID", *sliders[PluginKnobs::Treble]);
    sliderAttachments[PluginKnobs::Output] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTPUT_ID", *sliders[PluginKnobs::Output]);

    sliderAttachments[PluginKnobs::LowCut] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWCUT_ID", *sliders[PluginKnobs::LowCut]);
    sliderAttachments[PluginKnobs::HighCut] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHCUT_ID", *sliders[PluginKnobs::HighCut]);

    toneStackToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "TONE_STACK_ON_ID", *toneStackToggle));
    normalizeToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "NORMALIZE_ID", *normalizeToggle));    
    irToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "CAB_ON_ID", *irToggle));    

    if(audioProcessor.getLastModelPath() != "null")
    {        
        audioProcessor.getLastModelName() == "Model File Missing!" ? modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::red) : modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);      
        modelNameBox->setText(audioProcessor.getLastModelName());
    }

    if(audioProcessor.getLastIrPath() != "null")
    {        
        audioProcessor.getLastIrName() == "IR File Missing!" ? irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::red) : irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);      
        irNameBox->setText(audioProcessor.getLastIrName());
    }

    int meterHeight = 135;
    int meterWidth = 18;
    meterIn.setBounds(Rectangle<int>(sliders[PluginKnobs::LowCut]->getX() - 25 - (meterWidth / 2), 
        sliders[PluginKnobs::LowCut]->getY() - (meterHeight / 6), meterWidth, meterHeight));

    meterOut.setBounds(Rectangle<int>(sliders[PluginKnobs::HighCut]->getX() + sliders[PluginKnobs::HighCut]->getWidth() 
        + 25, sliders[PluginKnobs::LowCut]->getY() - (meterHeight / 6), meterWidth, meterHeight));
    
}

NamJUCEAudioProcessorEditor::~NamJUCEAudioProcessorEditor()
{
    for(int sliderAtt = 0; sliderAtt <= 7; ++sliderAtt)
        sliderAttachments[sliderAtt] = nullptr;

    toneStackToggleAttachment = nullptr;
    normalizeToggleAttachment = nullptr;
    irToggleAttachment = nullptr;
}

//==============================================================================
void NamJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromString("FF121212"));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    //Temporary Labels
    g.drawFittedText("Input", sliders[PluginKnobs::Input]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Noise Gate", sliders[PluginKnobs::NoiseGate]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText(ngThreshold, sliders[PluginKnobs::NoiseGate]->getBounds().withHeight(24).translated(0, 75), juce::Justification::centred, 1);
    g.drawFittedText("Bass", sliders[PluginKnobs::Bass]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Middle", sliders[PluginKnobs::Middle]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Treble", sliders[PluginKnobs::Treble]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Output", sliders[PluginKnobs::Output]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Low-Cut", sliders[PluginKnobs::LowCut]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("High-Cut", sliders[PluginKnobs::HighCut]->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);

    g.drawFittedText("Model", modelNameBox->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);
    g.drawFittedText("Impulse Response", irNameBox->getBounds().withHeight(24).translated(0, -30), juce::Justification::centred, 1);

    g.drawFittedText("In", meterIn.getBounds().withHeight(24).withWidth(24).translated(-7, -30), juce::Justification::centred, 1);
    g.drawFittedText("Out", meterOut.getBounds().withHeight(24).withWidth(24).translated(-7, -30), juce::Justification::centred, 1);
}

void NamJUCEAudioProcessorEditor::resized()
{
    
}

void NamJUCEAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{   
    //Temporary solution for displaying Noise Gate threshold on the GUI
    if(slider == sliders[PluginKnobs::NoiseGate].get())
        if(int(slider->getValue()) < -100)
            ngThreshold = "OFF";
        else
            ngThreshold = std::to_string(int(slider->getValue())) + " dB";
        repaint();
}

void NamJUCEAudioProcessorEditor::timerCallback()
{

}

void NamJUCEAudioProcessorEditor::setToneStackEnabled(bool toneStackEnabled)
{    
    for(int slider = PluginKnobs::Bass; slider <= PluginKnobs::Treble; ++slider)
    {
        sliders[slider] -> setEnabled(toneStackEnabled);
        toneStackEnabled ? sliders[slider]->setAlpha(1.0f) : sliders[slider] -> setAlpha(0.3f);
    }
}

void NamJUCEAudioProcessorEditor::loadModelButtonClicked()
{
    juce::FileChooser chooser("Choose an model to load", File::getSpecialLocation(File::userDesktopDirectory), "*.nam", true, false);

    if (chooser.browseForFileToOpen())
    {		
        File model;
        model = chooser.getResult();
        audioProcessor.loadNamModel(model);
        modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
        modelNameBox->setText(model.getFileNameWithoutExtension());        
    }
}

void NamJUCEAudioProcessorEditor::loadIrButtonClicked()
{
    juce::FileChooser chooser("Choose an IR to load", File::getSpecialLocation(File::userDesktopDirectory), "*.wav", true, false);

    if (chooser.browseForFileToOpen())
    {		
        File impulseResponse;
        impulseResponse = chooser.getResult();
        audioProcessor.loadImpulseResponse(impulseResponse);
        irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
        irNameBox->setText(impulseResponse.getFileNameWithoutExtension());        
    }
}

void NamJUCEAudioProcessorEditor::initializeTextBox(const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height)
{
    textBox.reset(new juce::TextEditor(label));
    addAndMakeVisible(textBox.get());
    textBox->setMultiLine(false);
    textBox->setReturnKeyStartsNewLine(false);
    textBox->setReadOnly(true);
    textBox->setScrollbarsShown(true);
    textBox->setCaretVisible(true);
    textBox->setPopupMenuEnabled(true);
    textBox->setAlpha(0.9f);
    textBox->setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
    textBox->setBounds(x, y, width, height);
}

void NamJUCEAudioProcessorEditor::initializeButton(const juce::String label, std::unique_ptr<juce::TextButton>& button, int x, int y, int width, int height)
{
    button.reset(new juce::TextButton("LoadModelButton"));
    addAndMakeVisible(button.get());
    button->setButtonText("Load");
    button->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
    button->setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::snow);
    button->setBounds(x, y, width, height);
}

void NamJUCEAudioProcessorEditor::initializeFilters()
{
    int knobSize = 76;
    int xStart = sliders[PluginKnobs::Middle]->getX() + 20;
    int xOffsetMultiplier = 90;
    
    for(int slider = PluginKnobs::LowCut; slider <= PluginKnobs::HighCut; ++slider)
    {
        sliders[slider].reset(new juce::Slider("slider" + std::to_string(slider)));
        addAndMakeVisible(sliders[slider].get());
        sliders[slider]->setLookAndFeel(&lnf);
        sliders[slider]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[slider]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        sliders[slider]->setPopupDisplayEnabled(true, true, getTopLevelComponent());

        sliders[slider]->setBounds(xStart + ((slider - 6) * xOffsetMultiplier), 265, knobSize, knobSize);
    }
}