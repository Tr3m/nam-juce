#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FileComparator.h"

//==============================================================================
NamJUCEAudioProcessor::NamJUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::mono(), true)
        #endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                         ),
      apvts(*this, nullptr, "Params", createParameters()), lowCut(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20.0f, 1.0f)),
      highCut(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 1.0f)), presetManager(apvts),
      midiHandler(presetManager, valuesInternal[ValuesInternal::PRESET_CHANGED_VIA_MIDI], [&](const std::string& paramID){this->updateInternalStateValue(paramID);})
#endif
{
    stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(false));

    filterCuttofs[OutputFilters::LowCutF] = apvts.getRawParameterValue("LOWCUT_ID");
    filterCuttofs[OutputFilters::HighCutF] = apvts.getRawParameterValue("HIGHCUT_ID");

    this->exportParameters(parameterIDs, parameterNames);

    for (int i = 0; i < NUM_INTERNAL_VALUES; ++i)
        valuesInternal[i].addListener(this);
}

NamJUCEAudioProcessor::~NamJUCEAudioProcessor()
{
    for (int i = 0; i < NUM_INTERNAL_VALUES; ++i)
        valuesInternal[i].removeListener(this);
}

//==============================================================================
const juce::String NamJUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NamJUCEAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool NamJUCEAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool NamJUCEAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double NamJUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NamJUCEAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int NamJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NamJUCEAudioProcessor::setCurrentProgram(int index) {}

const juce::String NamJUCEAudioProcessor::getProgramName(int index)
{
    return {};
}

void NamJUCEAudioProcessor::changeProgramName(int index, const juce::String& newName) {}

//==============================================================================
void NamJUCEAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DBG("Prepare...");
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    myNAM.prepare(spec);
    myNAM.hookParameters(apvts);

    DBG("Setting slim size to: " + std::to_string(this->slimSize));
    myNAM.setSlimSize(this->slimSize);

    cab.prepare(spec);
    cab.hookParameters(apvts);

    tenBandEq.prepare(spec);
    tenBandEq.hookParameters(apvts);

    doubler.prepare(spec);

    lowCut.reset();
    lowCut.prepare(spec);
    highCut.reset();
    highCut.prepare(spec);

    meterInSource.resize(getTotalNumOutputChannels(), sampleRate * 0.1 / samplesPerBlock);
    meterOutSource.resize(getTotalNumOutputChannels(), sampleRate * 0.1 / samplesPerBlock);

    // Load last NAM Model
    if (lastModelPath != "null" && modelFound)
    {
        this->updateDirectoryModels(lastModelPath);
        this->loadNamModel(juce::File(lastModelPath));
    }
    // else
    //     myNAM.clearModel();

    // Load last IR
    if (lastIrPath != "null" && irFound)
    {
        this->updateDirectoryIRs(lastIrPath);
        this->loadImpulseResponse(juce::File(lastIrPath));
        irLoaded = true;
    }
    
    this->previousModelDir = "null";
    this->previousIrDir = "null";

    midiHandler.clearMappings();
    midiHandler.loadConfig(midiHandler.defaultMidiConfig, apvts);

    prepareCalled = true;
}

void NamJUCEAudioProcessor::loadFromPreset(juce::String modelPath, juce::String irPath)
{
    // this->suspendProcessing(true);

    if (modelPath != "null")
    {
        juce::File fileCheck{modelPath};
        if (!fileCheck.exists())
        {
            myNAM.clearModel();
            modelFound = false;
            lastModelName = "Model File Missing!";
            lastModelPath = modelPath.toStdString();
            this->isA2 = false;
        }
        else
        {
            modelFound = true;
            this->loadNamModel(juce::File(modelPath.toStdString()), false);
            // lastModelPath = modelPath.toStdString();
            // lastModelName = fileCheck.getFileNameWithoutExtension().toStdString();
        }
    }
    else
    {
        myNAM.clearModel();
        lastModelPath = "null";
        lastModelName = "";
        this->isA2 = false;
    }

    // Load last IR
    if (irPath != "null")
    {
        juce::File fileCheck{irPath};
        if (!fileCheck.exists())
        {
            clearIR();
            irFound = false;
            irLoaded = false;
            lastIrName = "IR File Missing!";
            lastIrPath = irPath.toStdString();
        }
        else
        {
            irFound = true;
            this->loadImpulseResponse(juce::File(irPath), false);
            irLoaded = true;
            // lastIrPath = irPath.toStdString();
            // lastIrName = fileCheck.getFileNameWithoutExtension().toStdString();
        }
    }
    else
    {
        clearIR();
        lastIrPath = "null";
        lastIrName = "";
    }

    DBG("Loaded: \nModel: " + lastModelName + "\nIR: " + lastIrName);

    // this->suspendProcessing(false);
}

bool NamJUCEAudioProcessor::loadNamModel(juce::File modelToLoad, bool suspendProcessing)
{
    std::string model_path = modelToLoad.getFullPathName().toStdString();
    this->previousModelDir = lastModelPath == "null" ? "null" : juce::File(lastModelPath).getParentDirectory().getFullPathName().toStdString();

    DBG("About to load Model: " + modelToLoad.getFullPathName());

    if (suspendProcessing)
        this->suspendProcessing(true);

    bool loaded = myNAM.loadModel(model_path);

    if (suspendProcessing)
        this->suspendProcessing(false);

    auto search_paths = apvts.state.getOrCreateChildWithName("search_paths", nullptr);
    lastModelSerachDir = modelToLoad.getParentDirectory().getFullPathName().toStdString();
    search_paths.setProperty("LastModelSearchDir", juce::String(lastModelSerachDir), nullptr);

    if (loaded)
    {
        this->modelFound = true;
        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        lastModelPath = model_path;
        lastModelName = modelToLoad.getFileNameWithoutExtension().toStdString();
        addons.setProperty("model_path", juce::String(lastModelPath), nullptr);

        this->isA2 = myNAM.isModelSlimmable();
        // this->modelIndex = directoryModelNames.indexOf(lastModelName);

        DBG("Loaded Model: " + lastModelName + (isA2 ? " (Slimmable)" : ""));

        this->updateDirectoryModels(lastModelPath);
    }
    // else 
    // {
    //     lastModelName = "";
    //     this->isA2 = false;        
    // }

    return loaded;
}

bool NamJUCEAudioProcessor::loadNamModel(int modelIndex, bool suspendProcessing)
{
    if (modelIndex < 0 || modelIndex >= directoryModelPaths.size())
    {
        this->lastModelName = "";
        this->isA2 = false;
        return false;
    }

    juce::File modelToLoad(directoryModelPaths[modelIndex]);

    return this->loadNamModel(modelToLoad, suspendProcessing);
}

void NamJUCEAudioProcessor::loadNextModel()
{
    if (directoryModelNames.size() > 1)
    {
        modelIndex = modelIndex + 1 >= directoryModelNames.size() ? 0 : modelIndex + 1; 

        if(loadNamModel(modelIndex))
            return;
        else
            loadNextModel();
    }
}

void NamJUCEAudioProcessor::loadPreviousModel()
{
    if (directoryModelNames.size() > 1)
    {
        modelIndex = modelIndex - 1 < 0 ? directoryModelNames.size() - 1 : modelIndex - 1;

        if(loadNamModel(modelIndex))
            return;
        else
            loadPreviousModel();
    }
}


bool NamJUCEAudioProcessor::getTriggerStatus()
{
    auto t_state = myNAM.getTrigger();
    return t_state->isGating();
}

void NamJUCEAudioProcessor::clearNAM()
{
    this->suspendProcessing(true);
    myNAM.clearModel();
    lastModelPath = "null";
    lastModelName = "null";
    this->isA2 = false;

    auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
    addons.setProperty("model_path", juce::String(lastModelPath), nullptr);

    this->suspendProcessing(false);
}

void NamJUCEAudioProcessor::setSlimmableSize(double size)
{
    myNAM.setSlimSize(size);
    this->slimSize = myNAM.getSlimSize();
}

bool NamJUCEAudioProcessor::loadImpulseResponse(juce::File irToLoad, bool suspendProcessing)
{
    std::string ir_path = irToLoad.getFullPathName().toStdString();
    this->previousIrDir = lastIrPath == "null" ? "null" : juce::File(lastIrPath).getParentDirectory().getFullPathName().toStdString();

    DBG("About to load IR: " + irToLoad.getFullPathName().toStdString());
    
    if (suspendProcessing)
        this->suspendProcessing(true);

    bool loaded = cab.loadImpulseResponse(irToLoad);
    
    if (suspendProcessing)
        this->suspendProcessing(false);

    auto search_paths = apvts.state.getOrCreateChildWithName("search_paths", nullptr);
    lastIrSerachDir = irToLoad.getParentDirectory().getFullPathName().toStdString();
    search_paths.setProperty("LastIrSearchDir", juce::String(lastIrSerachDir), nullptr);
    
    if (loaded)
    {
        this->irLoaded = true;
        this->irFound = true;

        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        lastIrPath = ir_path;
        lastIrName = irToLoad.getFileNameWithoutExtension().toStdString();
        addons.setProperty("ir_path", juce::String(lastIrPath), nullptr);

        // this->irIndex = directoryIrNames.indexOf(lastIrName);

        DBG("Loaded IR: " + irToLoad.getFileNameWithoutExtension());
        
        this->updateDirectoryIRs(lastIrPath);
    }
    // else
    // {
    //     this->lastIrName = "";
    // }

    return loaded;
}

bool NamJUCEAudioProcessor::loadImpulseResponse(int irIndex, bool suspendProcessing)
{
    if (irIndex < 0 || irIndex >= directoryIrPaths.size())
    {
        this->lastIrName = "";
        return false;
    }

    juce::File irToLoad(directoryIrPaths[irIndex]);

    return this->loadImpulseResponse(irToLoad, suspendProcessing);
}

void NamJUCEAudioProcessor::loadNextIR()
{
    if (directoryIrNames.size() > 1)
    {
        irIndex = irIndex + 1 >= directoryIrNames.size() ? 0 : irIndex + 1; 

        if(loadImpulseResponse(irIndex))
            return;
        else
            loadNextIR();
    }
}

void NamJUCEAudioProcessor::loadPreviousIR()
{
    if (directoryIrNames.size() > 1)
    {
        irIndex = irIndex - 1 < 0 ? directoryIrNames.size() - 1 : irIndex - 1;

        if(loadImpulseResponse(irIndex))
            return;
        else
            loadPreviousIR();
    }
}

juce::Value* NamJUCEAudioProcessor::getStateValue(int index)
{
    if (index > stateValues.size() - 1 || index < 0) 
        return nullptr;
    return &(stateValues[index]);
}

void NamJUCEAudioProcessor::updateDirectoryModels(const std::string& currentPath)
{
    juce::File modelToLoad(currentPath);

    auto modelDir = modelToLoad.getParentDirectory();

    if (modelDir.getFullPathName().toStdString() != this->previousModelDir)
    {
        DBG("Model directory changed");

        auto fileArray = modelDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.nam");
        
        FileComparator comparator;
        fileArray.sort(comparator);

        this->directoryModelNames.clear();
        this->directoryModelPaths.clear();

        for (juce::File f : fileArray)
        {
            this->directoryModelNames.add(f.getFileNameWithoutExtension());
            this->directoryModelPaths.add(f.getFullPathName());
        }
    }

    this->modelIndex = directoryModelNames.indexOf(lastModelName);
}

void NamJUCEAudioProcessor::updateDirectoryIRs(const std::string& currentPath)
{
    juce::File irToLoad(currentPath);

    auto irDir = irToLoad.getParentDirectory();

    if (irDir.getFullPathName().toStdString() != this->previousIrDir)
    {
        DBG("IR Directory changed");
        auto fileArray = irDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");

        FileComparator comparator;
        fileArray.sort(comparator);

        this->directoryIrNames.clear();
        this->directoryIrPaths.clear();

        for (juce::File f : fileArray)
        {
            this->directoryIrNames.add(f.getFileNameWithoutExtension());
            this->directoryIrPaths.add(f.getFullPathName());
        }
    }

    this->irIndex = directoryIrNames.indexOf(lastIrName);
}

void NamJUCEAudioProcessor::clearIR()
{
    cab.clearIR();
    irLoaded = false;
    lastIrPath = "null";
    lastIrName = "null";

    auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
    addons.setProperty("ir_path", juce::String(lastIrPath), nullptr);
}

bool NamJUCEAudioProcessor::getIrStatus()
{
    return irLoaded;
}

void NamJUCEAudioProcessor::loadLastModelAndIr()
{
    auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
    this->loadFromPreset(addons.getProperty("model_path", juce::String()), addons.getProperty("ir_path", juce::String()));
}

void NamJUCEAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NamJUCEAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
    #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

            // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

void NamJUCEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    meterInSource.measureBlock(buffer);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    midiHandler.processMidiBuffer(midiMessages);

    juce::dsp::AudioBlock<float> block(buffer);

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    this->updateStateValues();

    myNAM.processBlock(buffer);
    cab.processBlock(buffer);

    // Ten-Band EQ Module
    if (*apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))
        tenBandEq.process(buffer);

    // Do Dual Mono
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        channelDataRight[sample] = channelDataLeft[sample];


    // Filters
    if (filterCuttofs[OutputFilters::LowCutF]->load() > 20)
    {
        *lowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), filterCuttofs[OutputFilters::LowCutF]->load(), 1.0f);
        lowCut.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    if (filterCuttofs[OutputFilters::HighCutF]->load() < 20000)
    {
        *highCut.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filterCuttofs[OutputFilters::HighCutF]->load(), 1.0f);
        highCut.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    // Doubler
    if (*apvts.getRawParameterValue("DOUBLER_SPREAD_ID") > 0.0)
    {
        doubler.setDelayMs(*apvts.getRawParameterValue("DOUBLER_SPREAD_ID"));
        doubler.process(buffer);
    }

    meterOutSource.measureBlock(buffer);
}

void NamJUCEAudioProcessor::updateStateValues()
{
    stateValues[StateValues::EQ_BYPASS].setValue(juce::var(*apvts.getRawParameterValue("EQ_BYPASS_STATE_ID")));
    stateValues[StateValues::CAB_BYPASS].setValue(juce::var(*apvts.getRawParameterValue("CAB_ON_ID")));
    stateValues[StateValues::NORMALIZE].setValue(juce::var(*apvts.getRawParameterValue("NORMALIZE_ID")));
    stateValues[StateValues::TONESTACK_BYPASS].setValue(juce::var(*apvts.getRawParameterValue("TONE_STACK_ON_ID")));
    
    /*
     * In case of a midi toggle these will get the last value the MidiHandler set
     * using the updateInternalStateValue() callback, so there shouldn't be a second
     * listener trigger....
    */
    valuesInternal[ValuesInternal::PRESET_NEXT_CALLED].setValue(juce::var(*apvts.getRawParameterValue("PRESET_NEXT_ID")));
    valuesInternal[ValuesInternal::PRESET_PREV_CALLED].setValue(juce::var(*apvts.getRawParameterValue("PRESET_PREVIOUS_ID")));
    valuesInternal[ValuesInternal::MODEL_NEXT_CALLED].setValue(juce::var(*apvts.getRawParameterValue("MODEL_NEXT_ID")));
    valuesInternal[ValuesInternal::MODEL_PREV_CALLED].setValue(juce::var(*apvts.getRawParameterValue("MODEL_PREVIOUS_ID")));
    valuesInternal[ValuesInternal::IR_NEXT_CALLED].setValue(juce::var(*apvts.getRawParameterValue("IR_NEXT_ID")));
    valuesInternal[ValuesInternal::IR_PREV_CALLED].setValue(juce::var(*apvts.getRawParameterValue("IR_PREVIOUS_ID")));
}

void NamJUCEAudioProcessor::updateInternalStateValue(const std::string& paramID)
{
    auto* value = valuesDict[paramID];
    
    if (value != nullptr)
    {
        DBG("MIDI: Update value: " + juce::String(paramID) + " " + juce::String(*apvts.getRawParameterValue(paramID)));
        value->setValue((juce::var(*apvts.getRawParameterValue(paramID))));
    }        
}


//==============================================================================
bool NamJUCEAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NamJUCEAudioProcessor::createEditor()
{
    return new NamJUCEAudioProcessorEditor(*this);
}

//==============================================================================
void NamJUCEAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    for (int i = state.getNumChildren(); --i >= 0;)
    {
        auto child = state.getChild(i);
        if (ignoredParams.contains(juce::String(child["id"].toString())))
            state.removeChild(i, nullptr);
    }

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    xml->addTextElement("ModelPath");
    xml->addTextElement("ModelName");
    xml->addTextElement("IRPath");
    xml->addTextElement("IRName");
    xml->setAttribute("ModelPath", lastModelPath);
    xml->setAttribute("ModelName", lastModelName);
    xml->setAttribute("IRPath", lastIrPath);
    xml->setAttribute("IRName", lastIrName);
    xml->addTextElement("LastModelSearchDir");
    xml->setAttribute("LastModelSearchDir", lastModelSerachDir);
    xml->addTextElement("LastIrSearchDir");
    xml->setAttribute("LastIrSearchDir", lastIrSerachDir);

    xml->setAttribute("SlimSize", int(this->slimSize * 10));
    xml->setAttribute("WindowWidth", int(windowWidth));
    
    //Preferences
    xml->setAttribute("colourScheme", preferences.getColourSchemeIndex());
    xml->setAttribute("showA2inTextBox", preferences.showA2Indicator);

    copyXmlToBinary(*xml, destData);
    DBG(xml->toString());
    // std::cout << xml->toString().toStdString() << std::endl;
}

void NamJUCEAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    DBG("Set State Info...");
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if(xmlState->hasAttribute("SlimSize"))
            this->slimSize = double(xmlState->getIntAttribute("SlimSize") / 10.0);

        if(xmlState->hasAttribute("WindowWidth"))
            this->windowWidth = static_cast<float>(xmlState->getIntAttribute("WindowWidth"));

        if(xmlState->hasAttribute("colourScheme"))
            preferences.setColourScheme(xmlState->getIntAttribute("colourScheme"));

        if (xmlState->hasAttribute("showA2inTextBox"))
            preferences.showA2Indicator = xmlState->getBoolAttribute("showA2inTextBox");

        if (xmlState->hasTagName(apvts.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xmlState);

            for (int i = state.getNumChildren(); --i >= 0;)
            {
                auto child = state.getChild(i);
                if (ignoredParams.contains(juce::String(child["id"].toString())))
                    state.removeChild(i, nullptr);
            }
            
            // std:: cout << state.toXmlString().toStdString() << std::endl;
            apvts.replaceState(state);

            // Try to load last NAM Model
            try
            {
                lastModelPath = xmlState->getStringAttribute("ModelPath").toStdString();
                lastModelName = xmlState->getStringAttribute("ModelName").toStdString();

                if (lastModelName != "null")
                {
                    juce::File fileCheck{lastModelPath};
                    if (!fileCheck.exists())
                    {
                        lastModelName = "Model File Missing!";
                        modelFound = false;
                    }
                    else
                        modelFound = true;
                }
            }
            catch (const std::exception& e)
            {
                lastModelPath = "null";
                lastModelName = "";
                modelFound = false;
            }

            // Try to load last IR
            try
            {
                lastIrPath = xmlState->getStringAttribute("IRPath").toStdString();
                lastIrName = xmlState->getStringAttribute("IRName").toStdString();

                if (lastIrName != "null")
                {
                    juce::File fileCheck{lastIrPath};
                    if (!fileCheck.exists())
                    {
                        lastIrName = "IR File Missing!";
                        irFound = false;
                    }
                    else
                        irFound = true;
                }
            }
            catch (const std::exception& e)
            {
                lastIrPath = "null";
                lastIrName = "";
                irFound = false;
            }

            // Try to load last Model Search Directory
            try
            {
                lastModelSerachDir = xmlState->getStringAttribute("LastModelSearchDir").toStdString();

                if (lastModelSerachDir != "null")
                {
                    juce::File fileCheck{lastModelSerachDir};
                    if (!fileCheck.exists())
                        lastModelSerachDir = "null";
                }
            }
            catch (const std::exception& e)
            {
                lastModelSerachDir = "null";
            }

            // Try to load last IR Search Directory
            try
            {
                lastIrSerachDir = xmlState->getStringAttribute("LastIrSearchDir").toStdString();

                if (lastIrSerachDir != "null")
                {
                    juce::File fileCheck{lastIrSerachDir};
                    if (!fileCheck.exists())
                        lastIrSerachDir = "null";
                }
            }
            catch (const std::exception& e)
            {
                lastIrSerachDir = "null";
            }
        }
    }

    if (this->prepareCalled)
        this->prepareToPlay(getSampleRate(), getBlockSize());
}

// This is probably a bad idea...
void NamJUCEAudioProcessor::valueChanged(juce::Value & value)
{
    if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::PRESET_CHANGED_VIA_MIDI]))
    {
        this->loadLastModelAndIr();
        stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::PRESET_NEXT_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Next Preset");
            if(presetManager.loadNextPreset())
            {
                this->loadLastModelAndIr();
                stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
            }
        }
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::PRESET_PREV_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Prev Preset");
            if(presetManager.loadPreviousPreset())
            {
                this->loadLastModelAndIr();
                stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
            }
        }
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::MODEL_NEXT_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Next Model");
            this->loadNextModel();
            stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
        }
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::MODEL_PREV_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Prev Model");
            this->loadPreviousModel();
            stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
        }
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::IR_NEXT_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Next IR");
            this->loadNextIR();
            stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
        }
    }
    else if (value.refersToSameSourceAs(valuesInternal[ValuesInternal::IR_PREV_CALLED]))
    {
        if (value.getValue() == juce::var(true))
        {
            DBG("Prev IR");
            this->loadPreviousIR();
            stateValues[StateValues::PRESET_CHANGED].setValue(juce::var(!stateValues[StateValues::PRESET_CHANGED].getValue()));
        }
    }
            
}

juce::AudioProcessorValueTreeState::ParameterLayout NamJUCEAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    myNAM.createParameters(parameters);
    cab.createParameters(parameters);

    parameters.push_back(std::make_unique<juce::AudioParameterInt>("LOWCUT_ID", "Low Cut", 19, 2000, 19));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("HIGHCUT_ID", "High Cut", 200, 20001, 20001));

    auto normRange = NormalisableRange<float>(0.0, 20.0, 0.1f);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DOUBLER_SPREAD_ID", "Doubler Spread", normRange, 0.0)); 

    tenBandEq.pushParametersToTree(parameters);

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PRESET_NEXT_ID", "Preset Next", false, "Preset Next"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("PRESET_PREVIOUS_ID", "Preset Previous", false, "Preset Previous"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("MODEL_NEXT_ID", "Model Next", false, "Model Next"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("MODEL_PREVIOUS_ID", "Model Previous", false, "Model Previous"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("IR_NEXT_ID", "IR Next", false, "IR Next"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("IR_PREVIOUS_ID", "IR Previous", false, "IR Previous"));

    return {parameters.begin(), parameters.end()};
}

bool NamJUCEAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return supportsDouble;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NamJUCEAudioProcessor();
}


void NamJUCEAudioProcessor::exportParameters(juce::StringArray& ids, juce::StringArray& names)
{
    ids.clear();
    names.clear();

    auto params = this->getParameters();

    for (auto* param : params )
    {
        names.add(param->getName(32));
        ids.add(dynamic_cast<juce::AudioProcessorParameterWithID*>(param)->getParameterID());
    }
}
