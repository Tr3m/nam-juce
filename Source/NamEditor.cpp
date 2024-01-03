#include "NamEditor.h"

NamEditor::NamEditor(NamJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), eqEditor(p), topBar(p, [&]() { updateAfterPresetLoad(); })
{
    assetManager.reset(new AssetManager());

    //Meters 
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

    lnf.setColour (Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    lnf.setColour (Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    lnf.setColour (Slider::textBoxTextColourId, juce::Colours::ivory);

    //Setup sliders
    for(int slider = 0; slider < NUM_SLIDERS; ++slider)
    {
        sliders[slider].reset(new juce::Slider("slider" + std::to_string(slider)));
        addAndMakeVisible(sliders[slider].get());
        sliders[slider]->setLookAndFeel(&lnf);
        sliders[slider]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        sliders[slider]->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
        //sliders[slider]->setPopupDisplayEnabled(true, true, getTopLevelComponent());

        int size2 = 20;
        
        if(slider >= PluginKnobs::LowCut)
        {
            xStart = sliders[PluginKnobs::Middle]->getX();
            sliders[slider]->setBounds(xStart + ((slider - 6) * xOffsetMultiplier) - 10, 435, knobSize + size2, knobSize + size2 + 15);
            sliders[slider]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
            sliders[slider]->setTextValueSuffix(" Hz");
        }
        else
            sliders[slider]->setBounds(xStart + (slider * xOffsetMultiplier), 204, knobSize, knobSize);
            
            
    }

    sliders[PluginKnobs::NoiseGate]->addListener(this);

    //Tone Stack Toggle
    toneStackToggle.reset(new juce::ToggleButton("ToneStackToggleButton"));
    addAndMakeVisible(toneStackToggle.get());
    toneStackToggle->setBounds(sliders[PluginKnobs::Bass]->getX() + 30, sliders[PluginKnobs::Bass]->getY() + knobSize + 50, 100, 40);
    toneStackToggle->setButtonText("Tone Stack");
    toneStackToggle->onClick = [this]{setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));};
    toneStackToggle->setVisible(false);

    //Rerunning this for GUI Recustrunction upon reopning the plugin
    setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));

    //Normalize Toggle
    normalizeToggle.reset(new juce::ToggleButton("NormalizeToggleButton"));
    addAndMakeVisible(normalizeToggle.get());
    normalizeToggle->setBounds(toneStackToggle->getX() + 120, toneStackToggle->getY(), 100, 40);
    normalizeToggle->setButtonText("Normalize");
    normalizeToggle->setVisible(false);

    //IR Toggle
    irToggle.reset(new juce::ToggleButton("IRToggleButton"));
    addAndMakeVisible(irToggle.get());
    irToggle->setBounds(normalizeToggle->getX() + 120, normalizeToggle->getY(), 100, 40);
    irToggle->setButtonText("IR");
    irToggle->setVisible(false);

    //Model Name Box and Button
    initializeTextBox("ModelNameBox", modelNameBox, 90, sliders[PluginKnobs::Input]->getY() + 195  + screensOffset, 200, 28);    
    initializeButton("LoadModelButton", "Load", loadModelButton, modelNameBox->getX() + modelNameBox->getWidth() + 15, modelNameBox->getY() - 3, 48, 39);
    assetManager->setLoadButton(loadModelButton);
    loadModelButton->onClick = [this]{loadModelButtonClicked();};

    //IR Name Box and Button
    initializeTextBox("IRNameBox", irNameBox, 90, modelNameBox->getY() + 80, 200, 28);
    initializeButton("LoadIRButton", "Load", loadIRButton, irNameBox->getX() + irNameBox->getWidth() + 15, irNameBox->getY() - 3, 48, 39);
    loadIRButton->onClick = [this]{loadIrButtonClicked();};
    assetManager->setLoadButton(loadIRButton);    

    initializeButton("ClearModelBtn", "X", clearModelButton, loadModelButton->getX() + loadModelButton->getWidth() + 10, loadModelButton->getY(), 48, 39);
    clearModelButton->setVisible(audioProcessor.getNamModelStatus());
    assetManager->setClearButton(clearModelButton);
    clearModelButton->onClick = [this]
    {
        audioProcessor.clearNAM();
        clearModelButton->setVisible(audioProcessor.getNamModelStatus());
        modelNameBox->setText("");
        modelNameBox->clear();
    };

    initializeButton("ClearIRbtn", "X", clearIrButton, loadIRButton->getX() + loadIRButton->getWidth() + 10, loadIRButton->getY(), 48, 39);
    clearIrButton->setVisible(audioProcessor.getIrStatus());
    assetManager->setClearButton(clearIrButton);
    clearIrButton->onClick = [this]
    {
        audioProcessor.clearIR();
        clearIrButton->setVisible(audioProcessor.getIrStatus());
        irNameBox->setText("");
        irNameBox->clear();
    };

    //Hook slider and button attacments
    for(int slider = 0; slider < NUM_SLIDERS; ++slider)
        sliderAttachments[slider] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, sliderIDs[slider], *sliders[slider]);

    sliderAttachments[PluginKnobs::LowCut] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWCUT_ID", *sliders[PluginKnobs::LowCut]);
    sliderAttachments[PluginKnobs::HighCut] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHCUT_ID", *sliders[PluginKnobs::HighCut]);

    toneStackToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "TONE_STACK_ON_ID", *toneStackToggle));
    normalizeToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "NORMALIZE_ID", *normalizeToggle));    
    irToggleAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "CAB_ON_ID", *irToggle));    

    //Check the processor for Model and IR status upon reopening the UI
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

    toneStackButton.reset(new juce::ImageButton("ToneStackButton"));
    addAndMakeVisible(toneStackButton.get());
    toneStackButton->setBounds(sliders[PluginKnobs::Middle]->getX() + (sliders[PluginKnobs::Middle]->getWidth() / 2) - 50, sliders[PluginKnobs::Middle]->getY() + sliders[PluginKnobs::Middle]->getHeight() + 15, 100, 40);
    assetManager->setToggleButton(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"), AssetManager::Buttons::TONESTACK_BUTTON);
    toneStackButton->onClick = [this]
    {
        toneStackToggle->setToggleState(!toneStackToggle->getToggleState(), true);
        assetManager->setToggleButton(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"), AssetManager::Buttons::TONESTACK_BUTTON);
    };

    normalizeButton.reset(new juce::ImageButton("NormalizeButton"));
    addAndMakeVisible(normalizeButton.get());
    normalizeButton->setBounds(sliders[PluginKnobs::Treble]->getX() + (sliders[PluginKnobs::Treble]->getWidth() / 2) - 50, sliders[PluginKnobs::Treble]->getY() + sliders[PluginKnobs::Treble]->getHeight() + 15, 100, 40);
    assetManager->setToggleButton(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"), AssetManager::Buttons::NORMALIZE_BUTTON);
    normalizeButton->onClick = [this]
    {
        normalizeToggle->setToggleState(!normalizeToggle->getToggleState(), true);
        assetManager->setToggleButton(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"), AssetManager::Buttons::NORMALIZE_BUTTON);
    };

    irButton.reset(new juce::ImageButton("irButton"));
    addAndMakeVisible(irButton.get());
    irButton->setBounds(sliders[PluginKnobs::Output]->getX() + (sliders[PluginKnobs::Output]->getWidth() / 2) - 50, sliders[PluginKnobs::Output]->getY() + sliders[PluginKnobs::Output]->getHeight() + 15, 100, 40);
    assetManager->setToggleButton(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"), AssetManager::Buttons::IR_BUTTON);
    irButton->onClick = [this]
    {
        irToggle->setToggleState(!irToggle->getToggleState(), true);
        assetManager->setToggleButton(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"), AssetManager::Buttons::IR_BUTTON);
    };

    eqButton.reset(new juce::ImageButton("EQButton"));
    addAndMakeVisible(eqButton.get());
    eqButton->setBounds(sliders[PluginKnobs::NoiseGate]->getX() + (sliders[PluginKnobs::NoiseGate]->getWidth() / 2) - 50, sliders[PluginKnobs::NoiseGate]->getY() + sliders[PluginKnobs::NoiseGate]->getHeight() + 15, 100, 40);
    assetManager->setToggleButton(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"), AssetManager::Buttons::EQ_BUTTON);
    eqButton->setAlwaysOnTop(true);
    eqButton->toFront(false);
    
    addAndMakeVisible(&eqEditor);
    eqEditor.setVisible(false);
    
    
    eqButton->onClick = [this]
    {
        auto modifiers = juce::ModifierKeys::getCurrentModifiers();
        if (modifiers.isShiftDown() && !audioProcessor.eqModuleVisible)
        {
            eqEditor.toggleEq();
            assetManager->setToggleButton(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"), AssetManager::Buttons::EQ_BUTTON);            
        }
        else
        {
            //toneStackToggle->setToggleState(!toneStackToggle->getToggleState(), true);
            audioProcessor.eqModuleVisible = !audioProcessor.eqModuleVisible;
            eqEditor.setVisible(audioProcessor.eqModuleVisible);

            if(audioProcessor.eqModuleVisible)
            {
                eqButton->setBounds(getWidth() - 43, 25, 20, 20);
                eqButton->setImages(false, true, false, xIcon, 0.7f, juce::Colours::transparentWhite, xIcon, 1.0f, juce::Colours::transparentWhite, xIcon, 0.65f, juce::Colours::transparentWhite, 0);
            }
            else
            {
                eqButton->setBounds(sliders[PluginKnobs::NoiseGate]->getX() + (sliders[PluginKnobs::NoiseGate]->getWidth() / 2) - 50, sliders[PluginKnobs::NoiseGate]->getY() + sliders[PluginKnobs::NoiseGate]->getHeight() + 15, 100, 40);
                assetManager->setToggleButton(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"), AssetManager::Buttons::EQ_BUTTON);
            }

            setMeterPosition(!audioProcessor.eqModuleVisible);
        }
    };

    eqButton->toFront(true);
    meterIn.toFront(true);
    meterOut.toFront(true);

    addAndMakeVisible(&topBar);

    startTimer(30);    
}

NamEditor::~NamEditor()
{
    for(int sliderAtt = 0; sliderAtt < NUM_SLIDERS; ++sliderAtt)
        sliderAttachments[sliderAtt] = nullptr;

    toneStackToggleAttachment = nullptr;
    normalizeToggleAttachment = nullptr;
    irToggleAttachment = nullptr;
}

void NamEditor::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromString("FF121212"));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    g.drawImageAt(assetManager->getBackground(), 0, 0);
    g.drawImageAt(assetManager->getScreens(), 0, 0);

    //// TODO: Move this into a dedicated component with its own timer
    g.drawImageAt(led_to_draw, 296, 168);
}

void NamEditor::resized()
{
    eqEditor.setBounds(getLocalBounds());
    
    setMeterPosition(!audioProcessor.eqModuleVisible); // Need to change this when more modules are added...

    topBar.setBounds(0, 0, getWidth(), 40);
}

void NamEditor::timerCallback()
{
    //// TODO: Move this into a dedicated component with its own timer

    if(audioProcessor.getTriggerStatus() && static_cast<float>(*audioProcessor.apvts.getRawParameterValue("NGATE_ID")) > -101.0)
        led_to_draw = led_on;
    else
        led_to_draw = led_off;
    repaint();
}

void NamEditor::sliderValueChanged(juce::Slider* slider)
{
    
}

void NamEditor::setToneStackEnabled(bool toneStackEnabled)
{    
    for(int slider = PluginKnobs::Bass; slider <= PluginKnobs::Treble; ++slider)
    {
        sliders[slider] -> setEnabled(toneStackEnabled);
        toneStackEnabled ? sliders[slider]->setAlpha(1.0f) : sliders[slider] -> setAlpha(0.3f);
    }
}

void NamEditor::loadModelButtonClicked()
{
    juce::FileChooser chooser("Choose an model to load", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.nam", true, false);

    if (chooser.browseForFileToOpen())
    {		
        juce::File model;
        model = chooser.getResult();
        audioProcessor.loadNamModel(model);
        modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
        modelNameBox->setText(model.getFileNameWithoutExtension());
    }

    clearModelButton->setVisible(audioProcessor.getNamModelStatus());
}

void NamEditor::loadIrButtonClicked()
{
    juce::FileChooser chooser("Choose an IR to load", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav", true, false);

    if (chooser.browseForFileToOpen())
    {		
        juce::File impulseResponse;
        impulseResponse = chooser.getResult();
        audioProcessor.loadImpulseResponse(impulseResponse);
        irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);
        irNameBox->setText(impulseResponse.getFileNameWithoutExtension());        
    }

    clearIrButton->setVisible(audioProcessor.getIrStatus());
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
    //textBoxFont.setBold(true);
    textBox->setFont(textBoxFont);
    textBox->setAlpha(0.8f);
    textBox->setBounds(x, y, width, height);    
}

void NamEditor::initializeButton(const juce::String label, const juce::String buttonText, std::unique_ptr<juce::ImageButton>& button, int x, int y, int width, int height)
{
    button.reset(new juce::ImageButton(label));
    addAndMakeVisible(button.get());
    button->setBounds(x, y, width, height);
}

void NamEditor::setMeterPosition(bool isOnMainScreen)
{
    if(isOnMainScreen)
    {
        meterlnf.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::ivory);
        meterlnf.setColour(foleys::LevelMeter::lmMeterOutlineColour, juce::Colours::transparentWhite);
        meterlnf.setColour(foleys::LevelMeter::lmMeterBackgroundColour, juce::Colours::transparentWhite);
        meterIn.setLookAndFeel(&meterlnf);
        meterOut.setLookAndFeel(&meterlnf);

        int meterHeight = 172;
        int meterWidth = 18;
        meterIn.setBounds(juce::Rectangle<int>(26, 174, meterWidth, meterHeight));
        meterOut.setBounds(juce::Rectangle<int>(getWidth() - meterWidth - 21, 174, meterWidth, meterHeight));
    }
    else
    {
        meterlnf2.setColour(foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::ivory);
        meterIn.setLookAndFeel(&meterlnf2);
        meterOut.setLookAndFeel(&meterlnf2);
        
        int meterHeight = 255;
        int meterWidth = 20;  
        meterIn.setBounds(20, (getHeight() / 2) - (meterHeight / 2) + 10, meterWidth, meterHeight);
        meterOut.setBounds(getWidth() - 30, (getHeight() / 2) - (meterHeight / 2) + 10, meterWidth, meterHeight);
    }
}

void NamEditor::updateAfterPresetLoad()
{
    if(audioProcessor.eqModuleVisible)
        eqButton->setImages(false, true, false, xIcon, 0.7f, juce::Colours::transparentWhite, xIcon, 1.0f, juce::Colours::transparentWhite, xIcon, 0.65f, juce::Colours::transparentWhite, 0);
    else
        assetManager->setToggleButton(eqButton, *audioProcessor.apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"), AssetManager::Buttons::EQ_BUTTON);

    eqEditor.updateGraphics();

    assetManager->setToggleButton(normalizeButton, *audioProcessor.apvts.getRawParameterValue("NORMALIZE_ID"), AssetManager::Buttons::NORMALIZE_BUTTON);
    assetManager->setToggleButton(toneStackButton, *audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID"), AssetManager::Buttons::TONESTACK_BUTTON);
    setToneStackEnabled(bool(*audioProcessor.apvts.getRawParameterValue("TONE_STACK_ON_ID")));
    assetManager->setToggleButton(irButton, *audioProcessor.apvts.getRawParameterValue("CAB_ON_ID"), AssetManager::Buttons::IR_BUTTON);

    auto addons = audioProcessor.apvts.state.getOrCreateChildWithName ("addons", nullptr);
    //DBG(addons.getProperty ("model_path", juce::String()).toString());
    //DBG(addons.getProperty ("ir_path", juce::String()).toString());

    audioProcessor.loadFromPreset(addons.getProperty ("model_path", juce::String()), addons.getProperty ("ir_path", juce::String()));               
    
    //Check the processor for Model and IR status after loading preset.
    if(audioProcessor.getLastModelPath() != "null")
    {        
        audioProcessor.getLastModelName() == "Model File Missing!" ? modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::red) : modelNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);      
        modelNameBox->setText(audioProcessor.getLastModelName());
    }
    else
    {
        modelNameBox->setText("");
    }

    if(audioProcessor.getLastIrPath() != "null")
    {        
        audioProcessor.getLastIrName() == "IR File Missing!" ? irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::red) : irNameBox->setColour(juce::TextEditor::textColourId, juce::Colours::snow);      
        irNameBox->setText(audioProcessor.getLastIrName());
    } 
    else
    {
        irNameBox->setText("");
    }

    clearModelButton->setVisible(audioProcessor.getNamModelStatus());       
    clearIrButton->setVisible(audioProcessor.getIrStatus()); 
}