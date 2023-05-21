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
                        lowCut(dsp::IIR::Coefficients<double>::makeHighPass(44100, 20.0f, 1.0f)),
                        highCut(dsp::IIR::Coefficients<double>::makeLowPass(44100, 20000.0f, 1.0f))
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
    
    lowCut.reset();
    lowCut.prepare(spec);
    highCut.reset();
    highCut.prepare(spec);

    fpBuffer.setSize(1, samplesPerBlock, false, false, false);
    fpBuffer.clear();

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
        cab.loadImpulseResponse(juce::File(lastIrPath), dsp::Convolution::Stereo::no, dsp::Convolution::Trim::no, 0, dsp::Convolution::Normalise::yes);
        irLoaded = true;
    }
}

void NamJUCEAudioProcessor::loadNamModel(juce::File modelToLoad)
{
    std::string model_path = modelToLoad.getFullPathName().toStdString();
    this->suspendProcessing(true);
    myNAM.loadModel(model_path);
    this->suspendProcessing(false);

    lastModelPath = model_path;
    lastModelName = modelToLoad.getFileNameWithoutExtension().toStdString();
}

bool NamJUCEAudioProcessor::getIrStatus()
{
    return irLoaded;
}

void NamJUCEAudioProcessor::loadImpulseResponse(juce::File irToLoad)
{
    std::string ir_path = irToLoad.getFullPathName().toStdString();
    cab.loadImpulseResponse(irToLoad, dsp::Convolution::Stereo::no, dsp::Convolution::Trim::no, 0, dsp::Convolution::Normalise::yes);
    irLoaded = true;
    lastIrPath = ir_path;
    lastIrName = irToLoad.getFileNameWithoutExtension().toStdString();
}

void NamJUCEAudioProcessor::clearIR()
{
    cab.reset();
    irLoaded = false;
    lastIrPath = "null";
    lastIrName = "null";
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

void NamJUCEAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
   

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

void NamJUCEAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages)
{
    meterInSource.measureBlock(buffer);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<double> block(buffer);

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    myNAM.processBlock(buffer, totalNumInputChannels, totalNumOutputChannels);

    //TODO: Change this ASAP! This is a HORRIBLE way of doing it.
    juce::dsp::AudioBlock<float> fpBlock(fpBuffer);
    auto* fpData = fpBuffer.getWritePointer(0);
    for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
        fpData[sample] = channelDataLeft[sample];

    if(bool(*apvts.getRawParameterValue("CAB_ON_ID")) && irLoaded)
    {
        cab.process(juce::dsp::ProcessContextReplacing<float>(fpBlock));
        if(irFound)
            fpBuffer.applyGain(juce::Decibels::decibelsToGain(9.0f));
    }

    //Do Dual Mono
    for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        channelDataLeft[sample] = fpData[sample];
        channelDataRight[sample] = fpData[sample];
    }

    *lowCut.state = *dsp::IIR::Coefficients<double>::makeHighPass(getSampleRate(), *apvts.getRawParameterValue("LOWCUT_ID"), 1.0f);
    *highCut.state = *dsp::IIR::Coefficients<double>::makeLowPass(getSampleRate(), *apvts.getRawParameterValue("HIGHCUT_ID"), 1.0f);

    lowCut.process(juce::dsp::ProcessContextReplacing<double>(block));
    highCut.process(juce::dsp::ProcessContextReplacing<double>(block));

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
    myNAM.clear();
    lastModelPath = "null";
    lastModelName = "null";
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
