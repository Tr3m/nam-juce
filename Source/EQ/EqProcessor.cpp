#include "EqProcessor.h"

EqProcessor::EqProcessor() {}

EqProcessor::~EqProcessor() {}

void EqProcessor::prepare(const juce::dsp::ProcessSpec& _spec)
{
    spec = _spec;

    for (int band = 0; band <= 9; ++band)
    {
        tbBands[band].reset();
        tbBands[band].prepare(spec);
    }
}

void EqProcessor::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);

    // Input Gain
    buffer.applyGain(juce::Decibels::decibelsToGain(params[Parameters::Input]->load()));

    updateBands();
    for (int band = 0; band <= 9; ++band)
        tbBands[band].process(juce::dsp::ProcessContextReplacing<float>(block));

    // Output Gain
    buffer.applyGain(juce::Decibels::decibelsToGain(params[Parameters::Output]->load()));
}

void EqProcessor::updateBands()
{

    // EQ Bands
    for (int band = 0; band <= 9; ++band)
        *tbBands[band].state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            spec.sampleRate, bandFreqs[band], 1.0, juce::Decibels::decibelsToGain(params[band]->load()));
}

void EqProcessor::pushParametersToTree(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& parameters)
{
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("EQ_BYPASS_STATE_ID", "EQ_BYPASS_STATE", false, "EQ_BYPASS_STATE"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("EQ_INPUT_GAIN_ID", "EQ_INPUT_GAIN", -12.0, 12.0, 0.0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("EQ_OUTPUT_GAIN_ID", "EQ_OUTPUT_GAIN", -12.0, 12.0, 0.0));

    // Ten Band Eq
    for (int band = 0; band <= 9; ++band)
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
            "TEN_BAND_" + std::to_string(band + 1) + "_ID", "TEN_BAND_" + std::to_string(band + 1), -12.0, 12.0, 0.0));

    DBG("EQ Parameters pushed to apvts...");
}

void EqProcessor::hookParameters(juce::AudioProcessorValueTreeState& apvts)
{
    // EQ Bands
    for (int band = 0; band <= 9; ++band)
        params[band] = apvts.getRawParameterValue("TEN_BAND_" + std::to_string(band + 1) + "_ID");

    // Gain Knobs
    params[Parameters::Input] = apvts.getRawParameterValue("EQ_INPUT_GAIN_ID");
    params[Parameters::Output] = apvts.getRawParameterValue("EQ_OUTPUT_GAIN_ID");

    DBG("EQ Parameters Hooked");
}