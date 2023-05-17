#pragma once

#include "NeuralAmpModelerCore/NAM/dsp.h"
#include "NeuralAmpModelerCore/dsp/ImpulseResponse.h"
#include "NeuralAmpModelerCore/dsp/NoiseGate.h"
#include "NeuralAmpModelerCore/dsp/RecursiveLinearFilter.h"
#include "NeuralAmpModelerCore/dsp/dsp.h"
#include "NeuralAmpModelerCore/dsp/wav.h"

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

private:

};