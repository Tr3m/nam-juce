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
	: mNAM(nullptr)
{

}

NeuralAmpModeler::~NeuralAmpModeler()
{

}

void NeuralAmpModeler::prepare()
{
	//Test Load
    auto dspPath = std::filesystem::u8path("C:\\Users\\tr3m\\Desktop\\nam\\block_letter.nam");
	mNAM = get_dsp(dspPath);
}

void NeuralAmpModeler::processBlock(juce::AudioBuffer<double>& buffer, int inputChannels, int outputChannels)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = inputChannels;
    auto totalNumOutputChannels = outputChannels;


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    double** samplePointer = &channelDataLeft;
    
    if (mNAM != nullptr)
    {
        mNAM->process(samplePointer, samplePointer, 1, buffer.getNumSamples(), 1.0, 1.0, mNAMParams);
        mNAM->finalize_(buffer.getNumSamples());
    }

    //DO DUAL MONO
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        channelDataRight[sample] = channelDataLeft[sample];
}