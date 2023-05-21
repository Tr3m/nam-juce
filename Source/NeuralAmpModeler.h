#pragma once

#include "NeuralAmpModelerCore/NAM/namdsp.h"
#include "NeuralAmpModelerCore/dsp/ImpulseResponse.h"
#include "NeuralAmpModelerCore/dsp/NoiseGate.h"
#include "NeuralAmpModelerCore/dsp/RecursiveLinearFilter.h"
#include "NeuralAmpModelerCore/dsp/coredsp.h"
#include "NeuralAmpModelerCore/dsp/wav.h"
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class NeuralAmpModeler
{
public: 

	NeuralAmpModeler();
	~NeuralAmpModeler();

	void prepare(juce::dsp::ProcessSpec& spec);
	void loadModel(const std::string modelPath);

	void clear();
	bool isModelLoaded();

	void processBlock(juce::AudioBuffer<double>& buffer, int inputChannels, int outputChannels);

	void hookParameters(juce::AudioProcessorValueTreeState&);

	enum EParams
	{
		kInputLevel = 0,
		kNoiseGateThreshold,
		kToneBass,
		kToneMid,
		kToneTreble,
		kOutputLevel,
		
		kEQActive,
		kOutNorm,
		kIRToggle,
		kNumParams
	};


private:
	double sampleRate;
	juce::AudioBuffer<double> outputBuffer; 

	//Parameter Pointers
	std::atomic<float>* params[8];	

	// Noise gates
	dsp::noise_gate::Trigger mNoiseGateTrigger;
	dsp::noise_gate::Gain mNoiseGateGain;
	bool noiseGateActive {false};

	//Noise Gate Parameters
	const double time = 0.01;
	const double ratio = 0.1; // Quadratic...
    const double openTime = 0.005;
    const double holdTime = 0.01;
    const double closeTime = 0.05;

	std::unique_ptr<DSP> mNAM;
	bool outputNormalized {false};
	bool modelLoaded {false};

	// Tone Stack modules
	recursive_linear_filter::LowShelf mToneBass;
	recursive_linear_filter::Peaking mToneMid;
	recursive_linear_filter::HighShelf mToneTreble;

	bool toneStackActive {true};

	//Tone Stack Parameters
	const double bassFrequency = 150.0;
	const double midFrequency = 425.0;
	const double trebleFrequency = 1800.0;
	const double bassQuality = 0.707;
	double midQuality = 0.7;
	const double trebleQuality = 0.707;

	std::unordered_map<std::string, double> mNAMParams = { {"Input", 0.0}, {"Output", 0.0} };

// Private Functions
private:
	void updateParameters();
	double dB_to_linear(double db_value);
	void doDualMono(juce::AudioBuffer<double>& mainBuffer, double** input);
};