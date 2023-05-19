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

void NeuralAmpModeler::prepare(juce::dsp::ProcessSpec& spec)
{
    this->sampleRate = spec.sampleRate;

    outputBuffer.setSize(1, spec.maximumBlockSize, false, false, false);
    outputBuffer.clear();

	//Test Load
    auto dspPath = std::filesystem::u8path("C:\\Users\\Manos\\Desktop\\nam\\block_letter.nam");
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
    auto* outputData = outputBuffer.getWritePointer(0);
    
    double** samplePointer = &channelDataLeft;
    double** outputPointer = &outputData;

    /*
    //Noise Gate Trigger
    double** triggerOutput = samplePointer;
    if (noiseGateActive)
    {
        const namdsp::noise_gate::TriggerParams triggerParams(time, params[EParams::kNoiseGateThreshold]->load(), ratio, openTime, holdTime, closeTime);
        this->mNoiseGateTrigger.SetParams(triggerParams);
        this->mNoiseGateTrigger.SetSampleRate(sampleRate);
        triggerOutput = this->mNoiseGateTrigger.Process(samplePointer, 1, buffer.getNumSamples());
    }
   
    */    
    
    if (mNAM != nullptr)
    {
        mNAM->process(samplePointer, outputPointer, 1, buffer.getNumSamples(), dB_to_linear(params[EParams::kInputLevel]->load()), 
            dB_to_linear(params[EParams::kOutputLevel]->load()), mNAMParams);
        mNAM->finalize_(buffer.getNumSamples());
    }
    
    

    // Apply the noise gate    
    //double** gateGainOutput = noiseGateActive ? mNoiseGateGain.Process(output, 1, buffer.getNumSamples()) : output;

    //Check if TONESTACK is active here...

    // Translate params from knob 0-10 to dB.
    // Tuned ranges based on my ear. E.g. seems treble doesn't need nearly as
    // much swing as bass can use.
    const double bassGainDB = 4.0 * (params[EParams::kToneBass]->load() - 5.0); // +/- 20
    const double midGainDB = 3.0 * (params[EParams::kToneMid]->load() - 5.0); // +/- 15
    const double trebleGainDB = 2.0 * (params[EParams::kToneTreble]->load() - 5.0); // +/- 10

    const double bassFrequency = 150.0;
    const double midFrequency = 425.0;
    const double trebleFrequency = 1800.0;
    const double bassQuality = 0.707;
    // Wider EQ on mid bump up to sound less honky.
    const double midQuality = midGainDB < 0.0 ? 1.5 : 0.7;
    const double trebleQuality = 0.707;

    // Define filter parameters
    recursive_linear_filter::BiquadParams bassParams(sampleRate, bassFrequency, bassQuality, bassGainDB);
    recursive_linear_filter::BiquadParams midParams(sampleRate, midFrequency, midQuality, midGainDB);
    recursive_linear_filter::BiquadParams trebleParams(sampleRate, trebleFrequency, trebleQuality, trebleGainDB);

    // Apply tone stack
    // Set parameters
    this->mToneBass.SetParams(bassParams);
    this->mToneMid.SetParams(midParams);
    this->mToneTreble.SetParams(trebleParams);

    double** bassPointers = this->mToneBass.Process(outputPointer, 1, buffer.getNumSamples());
    double** midPointers = this->mToneMid.Process(bassPointers, 1, buffer.getNumSamples());
    double** treblePointers = this->mToneTreble.Process(midPointers, 1, buffer.getNumSamples());

    

    //DO DUAL MONO
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        channelDataRight[sample] = treblePointers[0][sample];
        channelDataLeft[sample] = treblePointers[0][sample];
    }
}

void NeuralAmpModeler::updateParameters()
{
    /*if(int(params[EParams::kNoiseGateThreshold]->load() == -101))
        noiseGateActive = false;
    else
        noiseGateActive = true;*/

    noiseGateActive = int(params[EParams::kNoiseGateThreshold]->load()) < -100 ? false : true;
}

double NeuralAmpModeler::dB_to_linear(double db_value)
{
    return std::pow(10.0, db_value / 20.0);
}