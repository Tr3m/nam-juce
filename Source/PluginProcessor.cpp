/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NamJUCEAudioProcessor::NamJUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Params", createParameters()),
                        lowCut(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20.0f, 1.0f)),
                        highCut(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 1.0f)),
                        presetManager(apvts)
#endif
{

}

NamJUCEAudioProcessor::~NamJUCEAudioProcessor()
{
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NamJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NamJUCEAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NamJUCEAudioProcessor::getProgramName (int index)
{
    return {};
}

void NamJUCEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NamJUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.numChannels = getNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;

    myNAM.prepare(spec);
    myNAM.hookParameters(apvts);

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

    //Load last NAM Model
    try
    {
        if(lastModelPath != "null")
            myNAM.loadModel(lastModelPath);
        else
            myNAM.clear();
    }
    catch(const std::exception& e)
    {
        myNAM.clear();
    }

    //Load last IR
    if(lastIrPath != "null")
    {
        cab.loadImpulseResponse(juce::File(lastIrPath), juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        irLoaded = true;
    }
}

void NamJUCEAudioProcessor::loadFromPreset(juce::String modelPath, juce::String irPath)
{
    this->suspendProcessing(true);
    
    if(modelPath != "null")
    {
        juce::File fileCheck{modelPath};
        if(!fileCheck.exists())
        {           
            myNAM.clear(); 
            lastModelName = "Model File Missing!"; 
            lastModelPath = modelPath.toStdString();  
        }
        else
        {
            myNAM.loadModel(modelPath.toStdString());
            lastModelPath = modelPath.toStdString();
            lastModelName = fileCheck.getFileNameWithoutExtension().toStdString();
        }
    }
    else
    {
        myNAM.clear();
        lastModelPath = "null";
        lastModelName = "";
    }

    //Load last IR
    if(irPath != "null")
    {        
        juce::File fileCheck{irPath};
        if(!fileCheck.exists())
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
            cab.loadImpulseResponse(juce::File(irPath), juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
            irLoaded = true;
            lastIrPath = irPath.toStdString();
            lastIrName = fileCheck.getFileNameWithoutExtension().toStdString();
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

void NamJUCEAudioProcessor::loadNamModel(juce::File modelToLoad)
{
    std::string model_path = modelToLoad.getFullPathName().toStdString();
    this->suspendProcessing(true);
    myNAM.loadModel(model_path);
    this->suspendProcessing(false);

    lastModelPath = model_path;
    lastModelName = modelToLoad.getFileNameWithoutExtension().toStdString();

    auto addons = apvts.state.getOrCreateChildWithName ("addons", nullptr);
    addons.setProperty ("model_path", juce::String(lastModelPath), nullptr);
}

bool NamJUCEAudioProcessor::getIrStatus()
{
    return irLoaded;
}

bool NamJUCEAudioProcessor::getTriggerStatus()
{
    auto t_state = myNAM.getTrigger();
    return t_state->isGating();
}


void NamJUCEAudioProcessor::loadImpulseResponse(juce::File irToLoad)
{
    std::string ir_path = irToLoad.getFullPathName().toStdString();
    cab.loadImpulseResponse(irToLoad, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
    irLoaded = true;
    lastIrPath = ir_path;
    lastIrName = irToLoad.getFileNameWithoutExtension().toStdString();

    auto addons = apvts.state.getOrCreateChildWithName ("addons", nullptr);    
    addons.setProperty ("ir_path", juce::String(lastIrPath), nullptr);
    
}

void NamJUCEAudioProcessor::clearIR()
{
    cab.reset();
    irLoaded = false;
    lastIrPath = "null";
    lastIrName = "null";

    auto addons = apvts.state.getOrCreateChildWithName ("addons", nullptr);    
    addons.setProperty ("ir_path", juce::String(lastIrPath), nullptr);
}

void NamJUCEAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NamJUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
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

    if(myNAM.isModelLoaded())
        myNAM.processBlock(buffer, totalNumInputChannels, totalNumOutputChannels);

    if(bool(*apvts.getRawParameterValue("CAB_ON_ID")) && irLoaded)
    {
        cab.process(juce::dsp::ProcessContextReplacing<float>(block));
        if(irFound)
            buffer.applyGain(juce::Decibels::decibelsToGain(9.0f));
    }

    //Ten-Band EQ Module
    if(*apvts.getRawParameterValue("EQ_BYPASS_STATE_ID"))
        tenBandEq.process(buffer);

    //Do Dual Mono
    for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
        channelDataRight[sample] = channelDataLeft[sample];


    // Filters
    *lowCut.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), *apvts.getRawParameterValue("LOWCUT_ID"), 1.0f);
    *highCut.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), *apvts.getRawParameterValue("HIGHCUT_ID"), 1.0f);

    lowCut.process(juce::dsp::ProcessContextReplacing<float>(block));
    highCut.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Doubler
    if(*apvts.getRawParameterValue("DOUBLER_SPREAD_ID") > 0.0)
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
    return new NamJUCEAudioProcessorEditor (*this);
}

//==============================================================================
void NamJUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
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

    copyXmlToBinary(*xml, destData);
}

void NamJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    DBG("Set State Info...");
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        
            //Try to load last NAM Model
            try
            {
                lastModelPath = xmlState->getStringAttribute("ModelPath").toStdString();
                lastModelName = xmlState->getStringAttribute("ModelName").toStdString();               

                if(lastModelName != "null")
                {
                    juce::File fileCheck{lastModelPath};
                    if(!fileCheck.exists())                    
                        lastModelName = "Model File Missing!";                                       
                }                  
            }
            catch(const std::exception& e)
            {
                lastModelPath = "null";
                lastModelName = "";
            }

            //Try to load last IR
            try
            {
                lastIrPath = xmlState->getStringAttribute("IRPath").toStdString();
                lastIrName = xmlState->getStringAttribute("IRName").toStdString();

                if(lastIrName != "null")
                {
                    juce::File fileCheck{lastIrPath};
                    if(!fileCheck.exists())
                    {
                        lastIrName = "IR File Missing!";   
                        irFound = false;          
                    }
                    else
                        irFound = true;
                }
            }
            catch(const std::exception& e)
            {
                lastIrPath = "null";
                lastIrName = "";
                irFound = false;
            }               
        }
}

bool NamJUCEAudioProcessor::getNamModelStatus()
{
    return myNAM.isModelLoaded();
}

void NamJUCEAudioProcessor::clearNAM()
{
    this->suspendProcessing(true);
    myNAM.clear();
    lastModelPath = "null";
    lastModelName = "null";

    auto addons = apvts.state.getOrCreateChildWithName ("addons", nullptr);    
    addons.setProperty ("model_path", juce::String(lastModelPath), nullptr);

    this->suspendProcessing(false);
}

juce::AudioProcessorValueTreeState::ParameterLayout NamJUCEAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("INPUT_ID", "INPUT", -20.0f, 20.0f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("NGATE_ID", "NGATE", -101.0f, 0.0f, -80.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("BASS_ID", "BASS", 0.0f, 10.0f, 5.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIDDLE_ID", "MIDDLE", 0.0f, 10.0f, 5.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("TREBLE_ID", "TREBLE", 0.0f, 10.0f, 5.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUT_ID", "OUTPUT", -40.0f, 40.0f, 0.0f));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("TONE_STACK_ON_ID", "TONE_STACK_ON", true, "TONE_STACK_ON"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("NORMALIZE_ID", "NORMALIZE", false, "NORMALIZE"));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("CAB_ON_ID", "CAB_ON", true, "CAB_ON"));

    parameters.push_back(std::make_unique<juce::AudioParameterInt>("LOWCUT_ID", "LOWCUT", 20, 2000, 20));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>("HIGHCUT_ID", "HIGHCUT", 200, 20000, 20000));

    auto normRange = NormalisableRange<float>(0.0, 20.0, 0.1f);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DOUBLER_SPREAD_ID", "DOUBLER_SPREAD", normRange, 0.0));

    parameters.push_back(std::make_unique<juce::AudioParameterBool>("SMALL_WINDOW_ID", "SMALL_WINDOW", false, "SMALL_WINDOW"));

    tenBandEq.pushParametersToTree(parameters);

    return { parameters.begin(), parameters.end() };
}

bool NamJUCEAudioProcessor::supportsDoublePrecisionProcessing() const
{
    return supportsDouble;
}

const std::string NamJUCEAudioProcessor::getLastModelPath()
{
    return lastModelPath;
}

const std::string NamJUCEAudioProcessor::getLastModelName()
{
    return lastModelName;
}

const std::string NamJUCEAudioProcessor::getLastIrPath()
{
    return lastIrPath;
}

const std::string NamJUCEAudioProcessor::getLastIrName()
{
    return lastIrName;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NamJUCEAudioProcessor();
}
