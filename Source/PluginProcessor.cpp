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
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Params", createParameters())
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
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());


    myNAM.processBlock(buffer, totalNumInputChannels, totalNumOutputChannels);


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        // ..do something to the data...
    }
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
    copyXmlToBinary(*xml, destData);
}

void NamJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));   
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

    return { parameters.begin(), parameters.end() };
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
