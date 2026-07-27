#include "IrProcessor.h"
#include <juce_audio_formats/juce_audio_formats.h>

IrProcessor::IrProcessor()
{

}

IrProcessor::~IrProcessor()
{

}

void IrProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    this->spec = spec;
    cab.reset(new juce::dsp::Convolution());
    cab->prepare(this->spec);
}

void IrProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (isCabOn->load() && impulseLoaded)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        if (cab != nullptr)
        {
            cab->process(juce::dsp::ProcessContextReplacing<float>(block));
            buffer.applyGain(juce::Decibels::decibelsToGain(6.0f));
        }
    }
}

bool IrProcessor::loadImpulseResponse(const juce::File& irFile)
{
    if (!this->isIrValidFormat(irFile))
        return false;
     
    cab->reset();
    cab->loadImpulseResponse(irFile, juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
    this->impulseLoaded = true;

    return true;
}

void IrProcessor::clearIR()
{
    cab->reset();
    this->impulseLoaded = false;
}

bool IrProcessor::isIrValidFormat(const juce::File& fileToLoad)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(fileToLoad));

    return reader ? true : false;
}

void IrProcessor::createParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& parameters)
{
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("CAB_ON_ID", "CAB_ON", true, "CAB_ON"));

    DBG("IrProcessor Parameters created!");
}

void IrProcessor::hookParameters (juce::AudioProcessorValueTreeState& apvts)
{
    this->isCabOn = apvts.getRawParameterValue("CAB_ON_ID");
}
