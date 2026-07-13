#include "PluginProcessor.h"
#include "PluginEditor.h"

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
      highCut(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 1.0f)), presetManager(apvts)
#endif
{
    filterCuttofs[OutputFilters::LowCutF] = apvts.getRawParameterValue("LOWCUT_ID");
    filterCuttofs[OutputFilters::HighCutF] = apvts.getRawParameterValue("HIGHCUT_ID");
}

NamJUCEAudioProcessor::~NamJUCEAudioProcessor() {}

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
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    myNAM.prepare(spec);
    myNAM.hookParameters(apvts);

    DBG("Setting slim size to: " + std::to_string(this->slimSize));
    myNAM.setSlimSize(this->slimSize);

    cab.reset();
    cab.prepare(spec);

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
    if (lastModelPath != "null")
        this->loadNamModel(juce::File(lastModelPath));
    else
        myNAM.clearModel();

    // Load last IR
    if (lastIrPath != "null" && irFound)
    {
        this->loadImpulseResponse(juce::File(lastIrPath));
        irLoaded = true;
    }

    prepareCalled = true;
}

void NamJUCEAudioProcessor::loadFromPreset(juce::String modelPath, juce::String irPath)
{
    this->suspendProcessing(true);

    if (modelPath != "null")
    {
        juce::File fileCheck{modelPath};
        if (!fileCheck.exists())
        {
            myNAM.clearModel();
            lastModelName = "Model File Missing!";
            lastModelPath = modelPath.toStdString();
        }
        else
        {
            this->loadNamModel(juce::File(modelPath.toStdString()));
            // lastModelPath = modelPath.toStdString();
            // lastModelName = fileCheck.getFileNameWithoutExtension().toStdString();
        }
    }
    else
    {
        myNAM.clearModel();
        lastModelPath = "null";
        lastModelName = "";
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
            this->loadImpulseResponse(juce::File(irPath));
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

    this->suspendProcessing(false);
}

bool NamJUCEAudioProcessor::loadNamModel(juce::File modelToLoad)
{
    std::string model_path = modelToLoad.getFullPathName().toStdString();

    DBG("About to load Model: " + modelToLoad.getFullPathName());

    this->suspendProcessing(true);
    bool loaded = myNAM.loadModel(model_path);
    this->suspendProcessing(false);

    auto search_paths = apvts.state.getOrCreateChildWithName("search_paths", nullptr);
    lastModelSerachDir = modelToLoad.getParentDirectory().getFullPathName().toStdString();
    search_paths.setProperty("LastModelSearchDir", juce::String(lastModelSerachDir), nullptr);

    auto modelDir = modelToLoad.getParentDirectory();
    const auto fileArray = modelDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.nam");

    this->directoryModelNames.clear();
    this->directoryModelPaths.clear();

    for (juce::File f : fileArray)
    {
        this->directoryModelNames.add(f.getFileNameWithoutExtension());
        this->directoryModelPaths.add(f.getFullPathName());
    }

    if (loaded)
    {
        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        lastModelPath = model_path;
        lastModelName = modelToLoad.getFileNameWithoutExtension().toStdString();
        addons.setProperty("model_path", juce::String(lastModelPath), nullptr);

        this->isA2 = myNAM.isModelSlimmable();

        DBG("Loaded Model: " + lastModelName + (isA2 ? " (Slimmable)" : ""));
    }
    else 
    {
        lastModelName = "";
        this->isA2 = false;        
    }

    return loaded;
}

bool NamJUCEAudioProcessor::loadNamModel(int modelIndex)
{
    if (modelIndex < 0 || modelIndex >= directoryModelPaths.size())
    {
        this->lastModelName = "";
        this->isA2 = false;
        return false;
    }

    juce::File modelToLoad(directoryModelPaths[modelIndex]);

    std::string model_path = modelToLoad.getFullPathName().toStdString();
    DBG("About to load: " + modelToLoad.getFullPathName());
    this->suspendProcessing(true);
    bool loaded = myNAM.loadModel(model_path);
    this->suspendProcessing(false);

    if (loaded)
    {
        lastModelName = modelToLoad.getFileNameWithoutExtension().toStdString();
        lastModelPath = model_path;
        this->isA2 = myNAM.isModelSlimmable();
        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        addons.setProperty("model_path", juce::String(lastModelPath), nullptr);
        this->modelIndex = modelIndex;

        DBG("Loaded Model: " + lastModelName + (isA2 ? " (Slimmable)" : ""));
    }
    else
    {
        lastModelName = "";
        this->isA2 = false;
    }

    return loaded;
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

bool NamJUCEAudioProcessor::loadImpulseResponse(juce::File irToLoad)
{
    std::string ir_path = irToLoad.getFullPathName().toStdString();

    auto search_paths = apvts.state.getOrCreateChildWithName("search_paths", nullptr);
    lastIrSerachDir = irToLoad.getParentDirectory().getFullPathName().toStdString();
    search_paths.setProperty("LastIrSearchDir", juce::String(lastIrSerachDir), nullptr);

    auto irDir = irToLoad.getParentDirectory();
    const auto fileArray = irDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");

    this->directoryIrNames.clear();
    this->directoryIrPaths.clear();

    for (juce::File f : fileArray)
    {
        this->directoryIrNames.add(f.getFileNameWithoutExtension());
        this->directoryIrPaths.add(f.getFullPathName());
    }

    DBG("About to load IR: " + irToLoad.getFullPathName().toStdString());

    if (isIrValidFormat(irToLoad))
    {

        this->suspendProcessing(true);
        this->clearIR();

        cab.loadImpulseResponse(irToLoad, juce::dsp::Convolution::Stereo::no,
                juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        
        this->suspendProcessing(false);


        irLoaded = true;
        irFound = true;

        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        lastIrPath = ir_path;
        lastIrName = irToLoad.getFileNameWithoutExtension().toStdString();
        addons.setProperty("ir_path", juce::String(lastIrPath), nullptr);

        DBG("Loaded IR: " + irToLoad.getFileNameWithoutExtension());

        return true;
    }
    else
    {
        lastIrName = "";
        return false;
    }
}

bool NamJUCEAudioProcessor::loadImpulseResponse(int irIndex)
{
    if (irIndex < 0 || irIndex >= directoryIrPaths.size())
    {
        this->lastIrName = "";
        return false;
    }

    juce::File irToLoad(directoryIrPaths[irIndex]);

    std::string ir_path = irToLoad.getFullPathName().toStdString();

    DBG("About to load IR: " + irToLoad.getFullPathName().toStdString());


    if (isIrValidFormat(irToLoad))
    {

        this->suspendProcessing(true);
        this->clearIR();

        cab.loadImpulseResponse(irToLoad, juce::dsp::Convolution::Stereo::no,
                juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        
        this->suspendProcessing(false);

        irLoaded = true;
        irFound = true;

        auto addons = apvts.state.getOrCreateChildWithName("addons", nullptr);
        lastIrPath = ir_path;
        lastIrName = irToLoad.getFileNameWithoutExtension().toStdString();
        addons.setProperty("ir_path", juce::String(lastIrPath), nullptr);
        this->irIndex = irIndex;

        DBG("Loaded IR: " + irToLoad.getFileNameWithoutExtension());

        return true;
    }
    else
    {
        lastIrName = "";
        return false;
    }
}

bool NamJUCEAudioProcessor::isIrValidFormat(juce::File f)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(f));

    return reader ? true : false;
}


void NamJUCEAudioProcessor::clearIR()
{
    cab.reset();
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

    juce::dsp::AudioBlock<float> block(buffer);

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    myNAM.processBlock(buffer);

    if (bool(*apvts.getRawParameterValue("CAB_ON_ID")) && irLoaded)
    {
        cab.process(juce::dsp::ProcessContextReplacing<float>(block));
        if (irFound)
            buffer.applyGain(juce::Decibels::decibelsToGain(6.0f));
    }

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

    copyXmlToBinary(*xml, destData);
}

void NamJUCEAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    DBG("Set State Info...");
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if(xmlState->hasAttribute("SlimSize"))
            this->slimSize = double(xmlState->getIntAttribute("SlimSize") / 10.0);

        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

            // Try to load last NAM Model
            try
            {
                lastModelPath = xmlState->getStringAttribute("ModelPath").toStdString();
                lastModelName = xmlState->getStringAttribute("ModelName").toStdString();

                if (lastModelName != "null")
                {
                    juce::File fileCheck{lastModelPath};
                    if (!fileCheck.exists())
                        lastModelName = "Model File Missing!";
                }
            }
            catch (const std::exception& e)
            {
                lastModelPath = "null";
                lastModelName = "";
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

juce::AudioProcessorValueTreeState::ParameterLayout NamJUCEAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    myNAM.createParameters(parameters);

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("CAB_ON_ID", "CAB_ON", true, "CAB_ON"));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("LOWCUT_ID", "LOWCUT", 19, 2000, 19));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("HIGHCUT_ID", "HIGHCUT", 200, 20001, 20001));

    auto normRange = NormalisableRange<float>(0.0, 20.0, 0.1f);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DOUBLER_SPREAD_ID", "DOUBLER_SPREAD", normRange, 0.0)); 

    tenBandEq.pushParametersToTree(parameters);

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
