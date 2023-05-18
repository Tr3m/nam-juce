#pragma once

#include "NeuralAmpModelerCore/NAM/dsp.h"
#include "NeuralAmpModelerCore/dsp/ImpulseResponse.h"
#include "NeuralAmpModelerCore/dsp/NoiseGate.h"
#include "NeuralAmpModelerCore/dsp/RecursiveLinearFilter.h"
#include "NeuralAmpModelerCore/dsp/dsp.h"
#include "NeuralAmpModelerCore/dsp/wav.h"
#include <JuceHeader.h>

const int kNumPresets = 1;

enum EParams
{
	// These need to be the first ones because I use their indices to place
	// their rects in the GUI.
	kInputLevel = 0,
	kNoiseGateThreshold,
	kToneBass,
	kToneMid,
	kToneTreble,
	kOutputLevel,
	// The rest is fine though.
	kNoiseGateActive,
	kEQActive,
	kOutNorm,
	kIRToggle,
	kNumParams
};

const int numKnobs = 6;

enum ECtrlTags
{
	kCtrlTagModelName = 0,
	kCtrlTagIRName,
	kCtrlTagInputMeter,
	kCtrlTagOutputMeter,
	kCtrlTagAboutBox,
	kCtrlTagOutNorm,
	kNumCtrlTags
};

class NeuralAmpModeler
{
public: 

	NeuralAmpModeler();
	~NeuralAmpModeler();

	void processBlock(juce::AudioBuffer<double>& buffer, int inputChannels, int outputChannels);

	void prepare();

private:

	std::unique_ptr<DSP> mNAM;

	std::unordered_map<std::string, double> mNAMParams = { {"Input", 0.0}, {"Output", 0.0} };
};