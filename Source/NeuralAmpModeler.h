#pragma once

#include "NeuralAmpModelerCore/NAM/dsp.h"
#include "NeuralAmpModelerCore/dsp/ImpulseResponse.h"
#include "NeuralAmpModelerCore/dsp/NoiseGate.h"
#include "NeuralAmpModelerCore/dsp/RecursiveLinearFilter.h"
#include "NeuralAmpModelerCore/dsp/dsp.h"
#include "NeuralAmpModelerCore/dsp/wav.h"
#include <JuceHeader.h>

class NeuralAmpModeler
{
public: 

	NeuralAmpModeler();
	~NeuralAmpModeler();

	void prepare(double _sampleRate);
	
	void processBlock(juce::AudioBuffer<double>& buffer, int inputChannels, int outputChannels);

	void hookParameters(juce::AudioProcessorValueTreeState&);
	void updateParameters();

	enum EParams
	{
		kInputLevel = 0,
		kNoiseGateThreshold,
		kToneBass,
		kToneMid,
		kToneTreble,
		kOutputLevel,
		
		kNoiseGateActive,
		kEQActive,
		kOutNorm,
		kIRToggle,
		kNumParams
	};

private:
	double sampleRate;

	//Parameter Pointers
	std::atomic<float>* params[6];
	bool noiseGateActive {false};

	// Noise gates
	namdsp::noise_gate::Trigger mNoiseGateTrigger;
	namdsp::noise_gate::Gain mNoiseGateGain;

	//Noise Gate Parameters
	const double time = 0.01;
	const double ratio = 0.1; // Quadratic...
    const double openTime = 0.005;
    const double holdTime = 0.01;
    const double closeTime = 0.05;

	std::unique_ptr<DSP> mNAM;

	// Tone stack modules
	recursive_linear_filter::LowShelf mToneBass;
	recursive_linear_filter::Peaking mToneMid;
	recursive_linear_filter::HighShelf mToneTreble;

	std::unordered_map<std::string, double> mNAMParams = { {"Input", 0.0}, {"Output", 0.0} };
};