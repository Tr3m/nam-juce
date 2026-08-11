#include "NamEditor.h"

NamEditor::NamEditor(NamJUCEAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), topBar(p, [&]() { updateAfterPresetLoad(); },
            [&](const juce::String& presetName) { showSaveDialog(presetName); }, [&](){ showMappingsComponent(); })
{
    assetManager.reset(new AssetManager());
        
    juce::Font lnfFont = fontBold;
    lnfFont.setSizeAndStyle(20.0f, juce::Font::FontStyleFlags::bold, 1.0f, 0.0f);
    
    buttonLnf.setTextFont(lnfFont);
    buttonLnf.setTextYOffset(1);
    buttonLnfGlow.setTextFont(lnfFont);
    buttonLnfGlow.setTextYOffset(1);
    buttonLnfGlow.setColour(CoolButtons::TextButton::ColourIds::textGlowColourId,
            juce::Colour::fromString("#FFffb400"));

    slimLnfOn.setColour(CoolButtons::Slider::ColourIds::thumbGlowColourId, juce::Colour::fromString("#FFffb400"));

    meterlnf.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::ivory);
    meterlnf.setColour(foleys::LevelMeter::lmMeterOutlineColour, juce::Colours::transparentWhite);
    meterlnf.setColour(foleys::LevelMeter::lmMeterBackgroundColour, juce::Colours::transparentWhite);
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

    int knobSize = 98;
    int xStart = 75;
    int xOffsetMultiplier = 140;

    lnf.setColour(Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    lnf.setColour(Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    lnf.setColour(Slider::textBoxTextColourId, juce::Colours::ivory);
    lnf.setColour(juce::PopupMenu::backgroundColourId, juce::Colour::fromString("FF121212").withAlpha(0.8f));

    // Setup sliders
    for (int slider = 0; slider < NUM_SLIDERS; ++slider)
    {
        sliders[slider].reset(new CustomSlider());
        addAndMakeVisible(sliders[slider].get());
        sliders[slider]->setLookAndFeel(&lnf);
        sliders[slider]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[slider]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        // sliders[slider]->setPopupDisplayEnabled(true, true, getTopLevelComponent());

        int size2 = 20;

        if (slider >= PluginKnobs::LowCut)
        {
            xStart = sliders[PluginKnobs::Middle]->getX();
            sliders[slider]->setBounds(xStart + ((slider - 6) * xOffsetMultiplier) - 10, 435, knobSize + size2, knobSize + size2 + 15);
            sliders[slider]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        }
        else
            sliders[slider]->setBounds(xStart + (slider * xOffsetMultiplier), 204, knobSize, knobSize);
    }

    sliders[PluginKnobs::LowCut]->setCustomSlider(CustomSlider::SliderTypes::Filters);
    sliders[PluginKnobs::HighCut]->setCustomSlider(CustomSlider::SliderTypes::Filters);
    sliders[PluginKnobs::Doubler]->setPopupDisplayEnabled(true, true, getTopLevelComponent());
    sliders[PluginKnobs::Doubler]->setCustomSlider(CustomSlider::SliderTypes::Doubler);
    sliders[PluginKnobs::Doubler]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    sliders[PluginKnobs::Doubler]->setBounds(sliders[PluginKnobs::Output]->getX(), 435, knobSize, knobSize);
    sliders[PluginKnobs::NoiseGate]->setPopupDisplayEnabled(true, true, getTopLevelComponent());
    sliders[PluginKnobs::NoiseGate]->setCustomSlider(CustomSlider::SliderTypes::Gate);
    sliders[PluginKnobs::NoiseGate]->setPopupDisplayEnabled(true, true, getTopLevelComponent());
    sliders[PluginKnobs::NoiseGate]->addListener(this);

    // Tone Stack Toggle
    toneStackToggle.reset(new juce::ToggleButton("ToneStackToggleButton"));
    addAndMakeVisible(toneStackToggle.get());
    toneStackToggle->setBounds(sliders[PluginKnobs::Bass]->getX() + 30, sliders[PluginKnobs::Bass]->getY() + knobSize + 50, 90, 40);
    toneStackToggle->setButtonText("Tone Stack");
    toneStackToggle->onClick = [this] { setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"))); };
    toneStackToggle->setVisible(false);

    // Rerunning this for GUI Recustrunction upon reopning the plugin
    setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));

    // Normalize Toggle
    normalizeToggle.reset(new juce::ToggleButton("NormalizeToggleButton"));
    addAndMakeVisible(normalizeToggle.get());
    normalizeToggle->setBounds(toneStackToggle->getX() + 120, toneStackToggle->getY(), 90, 40);
    normalizeToggle->setButtonText("Normalize");
    normalizeToggle->setVisible(false);

    // IR Toggle
    irToggle.reset(new juce::ToggleButton("IRToggleButton"));
    addAndMakeVisible(irToggle.get());
    irToggle->setBounds(normalizeToggle->getX() + 120, normalizeToggle->getY(), 90, 40);
    irToggle->setButtonText("IR");
    irToggle->setVisible(false);

    // Model Name Box and Button
    initializeTextBox("ModelNameBox", modelNameBox, 90, sliders[PluginKnobs::Input]->getY() + 195 + screensOffset, 200, 28);
    initializeButton(
        "LoadModelButton", "Load", loadModelButton, modelNameBox->getX() + modelNameBox->getWidth() + 15, modelNameBox->getY() - 3, 48, 39);
    assetManager->setLoadButton(loadModelButton);
    loadModelButton->onClick = [this] { loadModelButtonClicked(); };

    // IR Name Box and Button
    initializeTextBox("IRNameBox", irNameBox, 90, modelNameBox->getY() + 80, 200, 28);
    initializeButton("LoadIRButton", "Load", loadIRButton, irNameBox->getX() + irNameBox->getWidth() + 15, irNameBox->getY() - 3, 48, 39);
    loadIRButton->onClick = [this] { loadIrButtonClicked(); };
    assetManager->setLoadButton(loadIRButton);

    initializeButton("ClearModelBtn", "X", clearModelButton, loadModelButton->getX() + loadModelButton->getWidth() + 65, loadModelButton->getY() + 7, 25, 25);
    clearModelButton->setTooltip("Clear Model");
    clearModelButton->setVisible(audioProcessor.isModelLoaded());
    assetManager->setClearButton(clearModelButton);
    clearModelButton->onClick = [this]
    {
        audioProcessor.clearNAM();
        // clearModelButton->setVisible(audioProcessor.isModelLoaded());
        clearModelButton->setVisible(false); // TODO: Fix this
        slimSlider->setLookAndFeel(audioProcessor.isA2Model() ? &slimLnfOn : &slimLnfOff);
        modelNameBox->setText("");
        modelNameBox->clear();
        modelComboBox->clear(juce::NotificationType::dontSendNotification);
    };

    initializeButton("ClearIRbtn", "X", clearIrButton, loadIRButton->getX() + loadIRButton->getWidth() + 65, loadIRButton->getY() + 7, 25, 25);
    clearIrButton->setTooltip("Clear Impulse Response");
    clearIrButton->setVisible(audioProcessor.getIrStatus());
    assetManager->setClearButton(clearIrButton);
    clearIrButton->onClick = [this]
    {
        audioProcessor.clearIR();
        clearIrButton->setVisible(audioProcessor.getIrStatus());
        irNameBox->setText("");
        irNameBox->clear();
        irComboBox->clear(juce::NotificationType::dontSendNotification);
    };

    // Hook slider and button attacments
    for (int slider = 0; slider < NUM_SLIDERS; ++slider)
        sliderAttachments[slider] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, sliderIDs[slider], *sliders[slider]);

    sliderAttachments[PluginKnobs::LowCut] =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWCUT_ID", *sliders[PluginKnobs::LowCut]);
    sliderAttachments[PluginKnobs::HighCut] =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHCUT_ID", *sliders[PluginKnobs::HighCut]);
    sliderAttachments[PluginKnobs::Doubler] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DOUBLER_SPREAD_ID", *sliders[PluginKnobs::Doubler]);

    toneStackToggleAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "TONE_STACK_ON_ID", *toneStackToggle));
    normalizeToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "NORMALIZE_ID", *normalizeToggle));
    irToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "CAB_ON_ID", *irToggle));


    toneStackButton.reset(new juce::TextButton("TONESTACK"));
    addAndMakeVisible(toneStackButton.get());
    toneStackButton->setBounds(sliders[PluginKnobs::Middle]->getX() + (sliders[PluginKnobs::Middle]->getWidth() / 2) - 45 + togleButtonXOffset,
                               sliders[PluginKnobs::Middle]->getY() + sliders[PluginKnobs::Middle]->getHeight() + 15, 90, 40);

    setGlowButtonLnf(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"));
    toneStackButton->onClick = [this]
    {
        toneStackToggle->setToggleState(!toneStackToggle->getToggleState(), true);
        setGlowButtonLnf(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"));
    };

    normalizeButton.reset(new juce::TextButton("NORMALIZE"));
    addAndMakeVisible(normalizeButton.get());
    normalizeButton->setBounds(sliders[PluginKnobs::Treble]->getX() + (sliders[PluginKnobs::Treble]->getWidth() / 2) - 45 + togleButtonXOffset,
                               sliders[PluginKnobs::Treble]->getY() + sliders[PluginKnobs::Treble]->getHeight() + 15, 90, 40);
    setGlowButtonLnf(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"));
    normalizeButton->onClick = [this]
    {
        normalizeToggle->setToggleState(!normalizeToggle->getToggleState(), true);
        setGlowButtonLnf(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"));
    };

    irButton.reset(new juce::TextButton("CAB"));
    addAndMakeVisible(irButton.get());
    irButton->setBounds(sliders[PluginKnobs::Output]->getX() + (sliders[PluginKnobs::Output]->getWidth() / 2) - 45 + togleButtonXOffset,
                        sliders[PluginKnobs::Output]->getY() + sliders[PluginKnobs::Output]->getHeight() + 15, 90, 40);
    setGlowButtonLnf(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"));
    irButton->onClick = [this]
    {
        irToggle->setToggleState(!irToggle->getToggleState(), true);
        setGlowButtonLnf(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"));
    };

    eqButton.reset(new juce::TextButton("EQ"));
    addAndMakeVisible(eqButton.get());
    eqButton->setBounds(sliders[PluginKnobs::NoiseGate]->getX() + (sliders[PluginKnobs::NoiseGate]->getWidth() / 2) - 45 + togleButtonXOffset,
                        sliders[PluginKnobs::NoiseGate]->getY() + sliders[PluginKnobs::NoiseGate]->getHeight() + 15, 90, 40);
    setGlowButtonLnf(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));
    eqToggle.reset(new juce::ToggleButton("ToneStackToggleButton"));
    addAndMakeVisible(eqToggle.get());
    eqToggle->setBounds(eqButton->getX(), eqButton->getY() + eqButton->getHeight() + 10, 30, 30);
    eqToggle->setVisible(false);
    eqToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "EQ_BYPASS_STATE_ID", *eqToggle));
    
    eqButton->addMouseListener(this, false);
    
    // Slimmable Model Slider    
    slimSlider.reset(new CustomSlider(CustomSlider::SliderTypes::Slim_Slider));
    addAndMakeVisible(slimSlider.get());
    slimSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    slimSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    slimSlider->setPopupDisplayEnabled(true, true, getTopLevelComponent());
    slimSlider->setRange(0.0, 1.0, 0.1);
    slimSlider->addListener(this);
    slimSlider->setValue(audioProcessor.getSlimmableSize(), juce::NotificationType::dontSendNotification);
    int sliderWidth = normalizeButton->getWidth() - 20;
    slimSlider->setBounds(sliders[PluginKnobs::Input]->getX() + (sliders[PluginKnobs::Input]->getWidth() / 2) - 37, normalizeButton->getY() + normalizeButton->getHeight() / 3 - 13, sliderWidth, normalizeButton->getHeight());
    slimSlider->setLookAndFeel(audioProcessor.isA2Model() ? &slimLnfOn : &slimLnfOff);

    // Model Combo Box
    modelComboBox.reset(new juce::ComboBox("ModelComboBox"));
    addAndMakeVisible(modelComboBox.get());
    modelComboBox->addListener(this);
    modelComboBox->setAlpha(0.0f);
    modelComboBox->setBounds(modelNameBox->getBounds());
    modelComboBox->setLookAndFeel(&lnf);

    // IR Combo Box
    irComboBox.reset(new juce::ComboBox("IrComboBox"));
    addAndMakeVisible(irComboBox.get());
    irComboBox->addListener(this);
    irComboBox->setAlpha(0.0f);
    irComboBox->setBounds(irNameBox->getBounds());
    irComboBox->setLookAndFeel(&lnf);

    
    initializeButton("PrevModelButton", "<", prevModelButton,
            loadModelButton->getX() + loadModelButton->getWidth() + 5, loadModelButton->getY() + 7, 25, 25);
    
    prevModelButton->onClick = [this]
    {
        if (audioProcessor.isModelLoaded())
        {
            audioProcessor.loadPreviousModel();
            updateModelBox();
        }
    };

    initializeButton("NextModelButton", ">", nextModelButton,
            prevModelButton->getX() + prevModelButton->getWidth() + 5, prevModelButton->getY(), 25, 25);

    nextModelButton->onClick = [this]
    {
        if (audioProcessor.isModelLoaded())
        {
            audioProcessor.loadNextModel();
            updateModelBox();
        }
    };

    assetManager->setNextAndPrevButtons(prevModelButton, nextModelButton);


    initializeButton("PrevIrButton", "<", prevIrButton,
            loadIRButton->getX() + loadIRButton->getWidth() + 5, loadIRButton->getY() + 7, 25, 25);

    prevIrButton->onClick = [this]
    {
        if (audioProcessor.getIrStatus())
        {
            audioProcessor.loadPreviousIR();
            updateIrBox();
        }
    };


    initializeButton("NextIrButton", ">", nextIrButton,
            prevIrButton->getX() + prevIrButton->getWidth() + 5, prevIrButton->getY(), 25, 25);

    nextIrButton->onClick = [this]
    {
        if (audioProcessor.getIrStatus())
        {
            audioProcessor.loadNextIR();
            updateIrBox();
        }
    };

    assetManager->setNextAndPrevButtons(prevIrButton, nextIrButton);


    addAndMakeVisible(&topBar);
    topBar.setAlwaysOnTop(true);
    topBar.toFront(true);

    // Check the processor for Model and IR status upon reopening the UI
    if (audioProcessor.getLastModelPath() != "null")
        this->updateModelBox();

    if (audioProcessor.getLastIrPath() != "null")
        this->updateIrBox();

    if (audioProcessor.isModelLoaded())
        populateModelComboBox();

    if (audioProcessor.getIrStatus())
        populateIrComboBox();

    audioProcessor.getTrigger()->addValueListener(this);

    ledComponent.reset(new LedComponent());
    addAndMakeVisible(ledComponent.get());
    // ledComponent->setBounds(sliders[PluginKnobs::NoiseGate]->getBounds().translated(sliders[PluginKnobs::NoiseGate]->getWidth(), -28));
    ledComponent->setBounds(298, 169, 25, 25);

    for (auto& val : audioProcessor.getStateValuesArray())
        val.addListener(this);

    if (audioProcessor.eqModuleVisible)
        showEqModule();

}

NamEditor::~NamEditor()
{
    eqEditor = nullptr;
    presetDialog = nullptr;
    audioProcessor.getTrigger()->removeValueListener(this);

    for (auto& val : audioProcessor.getStateValuesArray())
        val.removeListener(this);

    for (int sliderAtt = 0; sliderAtt < NUM_SLIDERS; ++sliderAtt)
        sliderAttachments[sliderAtt] = nullptr;

    toneStackToggleAttachment = nullptr;
    normalizeToggleAttachment = nullptr;
    irToggleAttachment = nullptr;
    eqToggleAttachment = nullptr;
}

void NamEditor::paint(juce::Graphics& g)
{
    // DBG("Repaint at: " + g.getClipBounds().toString());
    g.fillAll(juce::Colour::fromString("FF121212"));

    g.setColour(juce::Colours::white);

    // Title Background Colour
    int titleWidth = getWidth() / 2 - 60;
    int titleHeight = 62;
    juce::Rectangle<int> titleArea (getWidth() / 2 - titleWidth / 2, 50, titleWidth, titleHeight);
    g.setColour(juce::Colour::fromString("#FFad7e55"));
    g.fillRect(titleArea);

    g.drawImageAt(assetManager->getBackground(), 0, 0);
    
    // Knob Labels
    fontRegular.setSizeAndStyle(22.0f, juce::Font::FontStyleFlags::plain, 1.0f, 0.0f);
    g.setFont(fontRegular);

    for (int i = 0; i < NUM_SLIDERS; ++i)
    {
        g.drawFittedText(sliderLabels[i], sliders[i]->getBounds().withHeight(fontRegular.getHeight())
                .translated(1, -34), juce::Justification::centred, 1);

    }

    g.drawFittedText("MODEL", modelNameBox->getBounds().withHeight(fontRegular.getHeight())
            .translated(6, -28), juce::Justification::centredLeft, 1);

    g.drawFittedText("IMPULSE RESPONSE", irNameBox->getBounds().withHeight(fontRegular.getHeight())
            .translated(6, -28), juce::Justification::centredLeft, 1);

}

void NamEditor::resized()
{
    int meterHeight = 172;
    int meterWidth = 18;
    meterIn.setBounds(juce::Rectangle<int>(26, 174, meterWidth, meterHeight));
    meterOut.setBounds(juce::Rectangle<int>(getWidth() - meterWidth - 21, 174, meterWidth, meterHeight));

    topBar.setBounds(0, 0, getWidth(), 40);
}

void NamEditor::sliderValueChanged(juce::Slider* slider) 
{
    if (slider == slimSlider.get())
    {
        DBG("Slim Size: " + std::to_string(slimSlider->getValue()));
        audioProcessor.setSlimmableSize(slimSlider->getValue());
    }
    else if (slider == sliders[PluginKnobs::NoiseGate].get())
    {
        if (slider->getValue() < -100.0)
            ledComponent->setLedOn(false);
    }
}


void NamEditor::comboBoxChanged (juce::ComboBox* comboBox)
{
    if (comboBox == modelComboBox.get())
    {
        if(audioProcessor.loadNamModel(modelComboBox->getSelectedId() - 1))
        {
            updateModelBox();
        }

        modelComboBox->setSelectedId(0, juce::NotificationType::dontSendNotification);
    }
    else if (comboBox == irComboBox.get())
    {
        if (audioProcessor.loadImpulseResponse(irComboBox->getSelectedId() - 1))
        {
            updateIrBox();
        }

        irComboBox->setSelectedId(0, juce::NotificationType::dontSendNotification);
    }
}

void NamEditor::valueChanged (Value& value)
{
    if (value.refersToSameSourceAs(*(audioProcessor.getTrigger()->getGatingValue())) && static_cast<float>(*audioProcessor.apvts.getRawParameterValue("NGATE_ID")) > -101.0)
       ledComponent->setLedOn(value.getValue());

    else if (value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::EQ_BYPASS)))
        setGlowButtonLnf(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));

    else if(value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::NORMALIZE)))
        setGlowButtonLnf(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"));

    else if(value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::CAB_BYPASS)))
        setGlowButtonLnf(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"));

    else if(value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::TONESTACK_BYPASS)))
    {
        setGlowButtonLnf(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"));
        setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));
    }

    else if (value.refersToSameSourceAs(*audioProcessor.getStateValue(NamJUCEAudioProcessor::StateValues::PRESET_CHANGED)))
    {
        this->updateAfterPresetLoad(true);
        topBar.getPresetManagerComponent()->updateAfterMidiLoad();
    }
}

void NamEditor::setToneStackEnabled(bool toneStackEnabled)
{
    for (int slider = PluginKnobs::Bass; slider <= PluginKnobs::Treble; ++slider)
    {
        sliders[slider]->setEnabled(toneStackEnabled);
        toneStackEnabled ? sliders[slider]->setAlpha(1.0f) : sliders[slider]->setAlpha(0.3f);
    }
}

void NamEditor::loadModelButtonClicked()
{
    auto searchLocation = audioProcessor.getLastModelSearchDirectory() == "null" ? juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                                                                                 : juce::File(audioProcessor.getLastModelSearchDirectory());

    juce::FileChooser chooser("Choose an model to load", searchLocation, "*.nam", true, false);

    if (chooser.browseForFileToOpen())
    {
        juce::File model;
        model = chooser.getResult();
        if (audioProcessor.loadNamModel(model))
        {
            updateModelBox();
        }

        populateModelComboBox();
    }

}

void NamEditor::loadIrButtonClicked()
{
    auto searchLocation = audioProcessor.getLastIrSearchDirectory() == "null" ? juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                                                                              : juce::File(audioProcessor.getLastIrSearchDirectory());

    juce::FileChooser chooser("Choose an IR to load", searchLocation, "*.wav", true, false);

    if (chooser.browseForFileToOpen())
    {
        juce::File impulseResponse;
        impulseResponse = chooser.getResult();
        if(audioProcessor.loadImpulseResponse(impulseResponse))
        {
            updateIrBox();
        }

        populateIrComboBox();
    }
}

void NamEditor::initializeTextBox(const juce::String label, std::unique_ptr<juce::TextEditor>& textBox, int x, int y, int width, int height)
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
    textBox->setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    textBox->setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    juce::Font textBoxFont;
    textBoxFont.setHeight(18.0f);
    // textBoxFont.setBold(true);
    textBox->setFont(textBoxFont);
    textBox->setAlpha(0.8f);
    textBox->setBounds(x, y, width, height);
}

void NamEditor::initializeButton(const juce::String label, const juce::String buttonText, std::unique_ptr<juce::ImageButton>& button, int x, int y,
                                 int width, int height)
{
    button.reset(new juce::ImageButton(label));
    addAndMakeVisible(button.get());
    button->setBounds(x, y, width, height);
}

void NamEditor::updateAfterPresetLoad(bool isFromMidi)
{
    setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));

    setGlowButtonLnf(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"));
    setGlowButtonLnf(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"));
    setGlowButtonLnf(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"));
    setGlowButtonLnf(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"));

    auto addons = audioProcessor.apvts.state.getOrCreateChildWithName("addons", nullptr);
    // DBG(addons.getProperty ("model_path", juce::String()).toString());
    // DBG(addons.getProperty ("ir_path", juce::String()).toString());

    if (!isFromMidi)
        audioProcessor.loadFromPreset(addons.getProperty("model_path", juce::String()), addons.getProperty("ir_path", juce::String()));

    // Check the processor for Model and IR status after loading preset.
    if (audioProcessor.getLastModelPath() != "null")
    {
        this->updateModelBox();
    }
    else
    {
        modelNameBox->setText("");
    }

    if (audioProcessor.getLastIrPath() != "null")
    {
        this->updateIrBox();
    }
    else
    {
        irNameBox->setText("");
    }

    clearModelButton->setVisible(audioProcessor.isModelLoaded());
    clearIrButton->setVisible(audioProcessor.getIrStatus());

    populateModelComboBox();
    populateIrComboBox();
}

void NamEditor::populateModelComboBox()
{
    modelComboBox->clear(juce::NotificationType::dontSendNotification);
    modelComboBox->addItemList(audioProcessor.getDirectoryModelNames(), 1);
} 

void NamEditor::populateIrComboBox()
{
    irComboBox->clear(juce::NotificationType::dontSendNotification);
    irComboBox->addItemList(audioProcessor.getDirectoryIrNames(), 1);
}


void NamEditor::updateModelBox()
{
    modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
    modelNameBox->setText((audioProcessor.isA2Model() ? "[A2] " : "") + audioProcessor.getLastModelName());
    modelNameBox->setCaretPosition(0);
    clearModelButton->setVisible(audioProcessor.isModelLoaded());
    modelComboBox->setTooltip(juce::String(audioProcessor.getLastModelName()));
    slimSlider->setLookAndFeel(audioProcessor.isA2Model() ? &slimLnfOn : &slimLnfOff);
}

void NamEditor::updateIrBox()
{
    irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
    irNameBox->setText(audioProcessor.getLastIrName());
    irNameBox->setCaretPosition(0);
    clearIrButton->setVisible(audioProcessor.getIrStatus());
    irComboBox->setTooltip(juce::String(audioProcessor.getLastIrName()));
}

void NamEditor::showSaveDialog(const juce::String& presetName)
{
    presetDialog.reset(new PresetDialogBoxWrapper(audioProcessor.getPresetManager(), topBar.getPresetManagerComponent(), presetDialog));
    addAndMakeVisible(presetDialog.get());
    presetDialog->setAlwaysOnTop(true);
    presetDialog->toFront(false);
    presetDialog->setInputFieldText(presetName);
    presetDialog->setBounds(getLocalBounds());
    presetDialog->grabKeyboardFocus();
}

void NamEditor::showEqModule()
{
    eqEditor.reset(new EqContainer(audioProcessor, eqEditor));
    addAndMakeVisible(eqEditor.get());
    audioProcessor.eqModuleVisible = true;
    eqEditor->setBounds(0, 0, 950, 650);
}

void NamEditor::showMappingsComponent()
{
    if(mappingsComp == nullptr)
    {
        mappingsComp.reset(new MidiMappingsComponentWrapper(audioProcessor, mappingsComp));
        addAndMakeVisible(mappingsComp.get());
        mappingsComp->toFront(true);
        mappingsComp->setAlwaysOnTop(true);
        mappingsComp->setBounds(0, 0, 950, 650);
    }
}


void NamEditor::mouseDown(const juce::MouseEvent& e)
{
}

void NamEditor::mouseUp(const juce::MouseEvent& e)
{
    if (e.eventComponent == eqButton.get())
    {
        if ((e.mods.isRightButtonDown() || e.mods.isShiftDown()) && !audioProcessor.eqModuleVisible)
            eqToggle->setToggleState(!eqToggle->getToggleState(), juce::NotificationType::sendNotification);
        else
            this->showEqModule();
    }
    else
    {
        if (e.mods.isRightButtonDown())
        {
            auto* menu = topBar.getSettingsRootMenu();

            if (menu != nullptr)
            {
                menu->setLookAndFeel(topBar.getMenuLookAndFeel());

                auto mousePos = juce::Desktop::getInstance()
                                    .getMainMouseSource()
                                    .getScreenPosition()
                                    .roundToInt();

                juce::Rectangle<int> target(mousePos.x, mousePos.y, 1, 1);

                menu->showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this).withTargetScreenArea(target),
                        [&](int selection) {topBar.setMenuSelectedId(selection);});
            }
        }
    }
}


void NamEditor::setGlowButtonLnf(std::unique_ptr<juce::TextButton>& button, bool shouldBeGlowing)
{
    button->setLookAndFeel(shouldBeGlowing ? &buttonLnfGlow : &buttonLnf);
}
