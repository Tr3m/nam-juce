#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class EqProcessor
{
public:
    EqProcessor();
    ~EqProcessor();

    void prepare (const juce::dsp::ProcessSpec& _spec);
    void process (juce::AudioBuffer<float>& buffer);

    void pushParametersToTree (std::vector<std::unique_ptr<juce::RangedAudioParameter>>& parameters);
    void hookParameters (juce::AudioProcessorValueTreeState&);

    enum Parameters
    {
        _31 = 0,
        _62,
        _125,
        _250,
        _500,
        _1K,
        _2K,
        _4K,
        _8K,
        _16K,
        Input,
        Output
    };

private:
    void updateBands ();

    std::atomic<float>* params[12];

    juce::dsp::ProcessSpec spec;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> tbBands[10];

    float bandFreqs[10] = {31.0, 62.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16000.0};
};