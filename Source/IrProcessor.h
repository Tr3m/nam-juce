#ifndef __IR_PROCESSOR_H__
#define __IR_PROCESSOR_H__

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class IrProcessor
{
public:
    IrProcessor();
    ~IrProcessor();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void processBlock(juce::AudioBuffer<float>& buffer);

    bool loadImpulseResponse(const juce::File& irFile);
    void clearIR();

    void createParameters (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& parameters);
    void hookParameters (juce::AudioProcessorValueTreeState&);

private:
    juce::dsp::ProcessSpec spec;
    std::unique_ptr<juce::dsp::Convolution> cab;

    bool impulseLoaded {false};
    std::atomic<float>* isCabOn;

    bool isIrValidFormat(const juce::File& fileToLoad);

};

#endif
