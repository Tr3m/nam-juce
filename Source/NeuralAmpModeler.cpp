#include <algorithm> // std::clamp
#include <cmath>
#include <filesystem>
#include <iostream>
#include <utility>

///#include "Colors.h"
///#include "IControls.h"
#include "NeuralAmpModelerCore/NAM/activations.h"
// clang-format off
// These includes need to happen in this order or else the latter won't know
// a bunch of stuff.
#include "NeuralAmpModeler.h"
///#include "IPlug_include_in_plug_src.h"
// clang-format on
#include "architecture.hpp"

NeuralAmpModeler::NeuralAmpModeler()
	: mNAM(nullptr),
    mNoiseGateTrigger(),
    mToneBass(),
    mToneMid(),
    mToneTreble()
{

}

NeuralAmpModeler::~NeuralAmpModeler()
{

}

void NeuralAmpModeler::prepare(double _sampleRate)
{
    this->sampleRate = _sampleRate;

	//Test Load
    auto dspPath = std::filesystem::u8path("/home/manos/Desktop/nam/block_letter.nam");
	mNAM = get_dsp(dspPath);
}

void NeuralAmpModeler::hookParameters(juce::AudioProcessorValueTreeState& apvts)
{
    params[EParams::kInputLevel] = apvts.getRawParameterValue("INPUT_ID");
    params[EParams::kNoiseGateThreshold] = apvts.getRawParameterValue("NGATE_ID");
    params[EParams::kToneBass] = apvts.getRawParameterValue("BASS_ID");
    params[EParams::kToneMid] = apvts.getRawParameterValue("MIDDLE_ID");
    params[EParams::kToneTreble] = apvts.getRawParameterValue("TREBLE_ID");
    params[EParams::kOutputLevel] = apvts.getRawParameterValue("OUTPUT_ID");

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
    
    double** samplePointer = &channelDataLeft;

    //Noise Gate Trigger
    double** triggerOutput = samplePointer;
    if (noiseGateActive)
    {
        const namdsp::noise_gate::TriggerParams triggerParams(time, params[EParams::kNoiseGateThreshold]->load(), ratio, openTime, holdTime, closeTime);
        this->mNoiseGateTrigger.SetParams(triggerParams);
        this->mNoiseGateTrigger.SetSampleRate(sampleRate);
        triggerOutput = this->mNoiseGateTrigger.Process(samplePointer, 1, buffer.getNumSamples());
    }
    
    if (mNAM != nullptr)
    {
        mNAM->process(samplePointer, samplePointer, 1, buffer.getNumSamples(), params[EParams::kInputLevel]->load(), params[EParams::kOutputLevel]->load(), mNAMParams);
        mNAM->finalize_(buffer.getNumSamples());
    }

    // Apply the noise gate
    //double** gateGainOutput = noiseGateActive ? mNoiseGateGain.Process(samplePointer, 1, buffer.getNumSamples()) : samplePointer;

    if(noiseGateActive)
        mNoiseGateGain.Process(samplePointer, 1, buffer.getNumSamples());

    //DO DUAL MONO
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        channelDataRight[sample] = channelDataLeft[sample];
}

void NeuralAmpModeler::updateParameters()
{
    /*if(int(params[EParams::kNoiseGateThreshold]->load() == -101))
        noiseGateActive = false;
    else
        noiseGateActive = true;*/

    noiseGateActive = int(params[EParams::kNoiseGateThreshold]->load() == -101) ? false : true;
}
