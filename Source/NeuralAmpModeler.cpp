#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <utility>

#include "NeuralAmpModelerCore/NAM/activations.h"
#include "NeuralAmpModeler.h"
#include "architecture.hpp"

NeuralAmpModeler::NeuralAmpModeler()
	: mNAM(nullptr),
    mNoiseGateTrigger(),
    mToneBass(),
    mToneMid(),
    mToneTreble()
{
    this->mNoiseGateTrigger.AddListener(&this->mNoiseGateGain);
}

NeuralAmpModeler::~NeuralAmpModeler()
{

}

void NeuralAmpModeler::prepare(juce::dsp::ProcessSpec& spec)
{
    this->sampleRate = spec.sampleRate;

    outputBuffer.setSize(1, spec.maximumBlockSize, false, false, false);
    outputBuffer.clear();
}

void NeuralAmpModeler::loadModel(const std::string modelPath)
{
    auto dspPath = std::filesystem::u8path(modelPath);
    mNAM = get_dsp(dspPath);
}

void NeuralAmpModeler::clear()
{
    mNAM = nullptr;
}

void NeuralAmpModeler::hookParameters(juce::AudioProcessorValueTreeState& apvts)
{
    params[EParams::kInputLevel] = apvts.getRawParameterValue("INPUT_ID");
    params[EParams::kNoiseGateThreshold] = apvts.getRawParameterValue("NGATE_ID");
    params[EParams::kToneBass] = apvts.getRawParameterValue("BASS_ID");
    params[EParams::kToneMid] = apvts.getRawParameterValue("MIDDLE_ID");
    params[EParams::kToneTreble] = apvts.getRawParameterValue("TREBLE_ID");
    params[EParams::kOutputLevel] = apvts.getRawParameterValue("OUTPUT_ID");

    params[EParams::kEQActive] = apvts.getRawParameterValue("TONE_STACK_ON_ID");
    params[EParams::kOutNorm] = apvts.getRawParameterValue("NORMALIZE_ID");

    DBG("Parameters Hooked!");
}

void NeuralAmpModeler::processBlock(juce::AudioBuffer<double>& buffer, int inputChannels, int outputChannels)
{
    updateParameters();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = inputChannels;
    auto totalNumOutputChannels = outputChannels;


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);
    auto* outputData = outputBuffer.getWritePointer(0);
    
    double** inputPointer = &channelDataLeft;
    double** outputPointer = &outputData;

    
    //Noise Gate Trigger
    double** triggerOutput = inputPointer;
    if (noiseGateActive)
    {
        const namdsp::noise_gate::TriggerParams triggerParams(time, params[EParams::kNoiseGateThreshold]->load(), ratio, openTime, holdTime, closeTime);
        this->mNoiseGateTrigger.SetParams(triggerParams);
        this->mNoiseGateTrigger.SetSampleRate(sampleRate);
        triggerOutput = this->mNoiseGateTrigger.Process(inputPointer, 1, buffer.getNumSamples());
    } 
    
    if (mNAM != nullptr)
    {
        mNAM->SetNormalize(outputNormalized);
        mNAM->process(triggerOutput, outputPointer, 1, buffer.getNumSamples(), dB_to_linear(params[EParams::kInputLevel]->load()), 
            dB_to_linear(params[EParams::kOutputLevel]->load()), mNAMParams);
        mNAM->finalize_(buffer.getNumSamples());
    }
    else
        outputPointer = triggerOutput;

    // Apply the noise gate    
    double** gateGainOutput = noiseGateActive ? mNoiseGateGain.Process(outputPointer, 1, buffer.getNumSamples()) : outputPointer;

    if(toneStackActive)
    {
        //Apply the tone stack
        double** bassPointers = this->mToneBass.Process(gateGainOutput, 1, buffer.getNumSamples());
        double** midPointers = this->mToneMid.Process(bassPointers, 1, buffer.getNumSamples());
        double** treblePointers = this->mToneTreble.Process(midPointers, 1, buffer.getNumSamples()); 

        doDualMono(buffer, treblePointers);
    } 
    else
        doDualMono(buffer, gateGainOutput);

    
}

void NeuralAmpModeler::updateParameters()
{
    outputNormalized = bool(params[EParams::kOutNorm]->load());

    //Noise Gate
    noiseGateActive = int(params[EParams::kNoiseGateThreshold]->load()) < -100 ? false : true;

    //Tone Stack
    toneStackActive = bool(params[EParams::kEQActive]->load());

    // Translate params from knob 0-10 to dB.
    // Tuned ranges based on my ear. E.g. seems treble doesn't need nearly as
    // much swing as bass can use.
    const double bassGainDB = 4.0 * (params[EParams::kToneBass]->load() - 5.0); // +/- 20
    const double midGainDB = 3.0 * (params[EParams::kToneMid]->load() - 5.0); // +/- 15
    const double trebleGainDB = 2.0 * (params[EParams::kToneTreble]->load() - 5.0); // +/- 10
    
    // Wider EQ on mid bump up to sound less honky.
    midQuality = midGainDB < 0.0 ? 1.5 : 0.7;    

    // Define filter parameters
    recursive_linear_filter::BiquadParams bassParams(sampleRate, bassFrequency, bassQuality, bassGainDB);
    recursive_linear_filter::BiquadParams midParams(sampleRate, midFrequency, midQuality, midGainDB);
    recursive_linear_filter::BiquadParams trebleParams(sampleRate, trebleFrequency, trebleQuality, trebleGainDB);

    // Set tone stack parameters
    this->mToneBass.SetParams(bassParams);
    this->mToneMid.SetParams(midParams);
    this->mToneTreble.SetParams(trebleParams);
}

double NeuralAmpModeler::dB_to_linear(double db_value)
{
    return std::pow(10.0, db_value / 20.0);
}

void NeuralAmpModeler::doDualMono(juce::AudioBuffer<double>& mainBuffer, double** input)
{
    auto channelDataLeft = mainBuffer.getWritePointer(0);
    auto channelDataRight = mainBuffer.getWritePointer(1);

    for (int sample = 0; sample < mainBuffer.getNumSamples(); ++sample)
    {
        channelDataRight[sample] = input[0][sample];
        channelDataLeft[sample] = input[0][sample];
    }
}