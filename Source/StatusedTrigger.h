#ifndef __STATUSED_TRIGGER_H__
#define __STATUSED_TRIGGER_H__

#include <cmath>
#include <unordered_set>
#include <vector>
#include <algorithm> // std::clamp
#include <cstring> // memcpy
#include <cmath> // pow
#include <sstream>

#include "../Modules/AudioDSPTools/dsp/dsp.h"
#include "../Modules/AudioDSPTools/dsp/NoiseGate.h"
#include <juce_data_structures/juce_data_structures.h>

extern const double dsp::noise_gate::MINIMUM_LOUDNESS_DB;
extern const double dsp::noise_gate::MINIMUM_LOUDNESS_POWER;

class StatusedTrigger : public dsp::noise_gate::Trigger
{
public:
    StatusedTrigger();
    DSP_SAMPLE** Process (DSP_SAMPLE** inputs, const size_t numChannels, const size_t numFrames) override;
    std::vector<std::vector<DSP_SAMPLE>> GetGainReduction() const { return this->mGainReductionDB; };
    void SetParams(const dsp::noise_gate::TriggerParams& params) { this->mParams = params; };
    void SetSampleRate(const double sampleRate) { this->mSampleRate = sampleRate; }
    std::vector<std::vector<DSP_SAMPLE>> GetGainReductionDB() const { return this->mGainReductionDB; };

    void AddListener(dsp::noise_gate::Gain* gain)
    {
        // This might be risky dropping a raw pointer, but I don't think that the
        // gain would be destructed, so probably ok.
        this->mGainListeners.insert(gain);
    }

    bool isGating() { return this->gating; };
    void addValueListener(juce::Value::Listener*);
    void removeValueListener(juce::Value::Listener*);
    juce::Value* getGatingValue() { return &isGatingValue; };

private:
    enum class State
    {
        MOVING = 0,
        HOLDING
    };

    double _GetGainReduction(const double levelDB) const
    {
        const double threshold = this->mParams.GetThreshold();
        // Quadratic gain reduction? :)
        return levelDB < threshold ? -(this->mParams.GetRatio()) * (levelDB - threshold) * (levelDB - threshold) : 0.0;
    }

    double _GetMaxGainReduction() const { return this->_GetGainReduction(dsp::noise_gate::MINIMUM_LOUDNESS_DB); }
    virtual void _PrepareBuffers (const size_t numChannels, const size_t numFrames) override;

    dsp::noise_gate::TriggerParams mParams;
    std::vector<State> mState; // One per channel
    std::vector<double> mLevel;

    // Hold the vectors of gain reduction for the block, in dB.
    // These can be given to the Gain object.
    std::vector<std::vector<DSP_SAMPLE>> mGainReductionDB;
    std::vector<double> mLastGainReductionDB;

    double mSampleRate;
    // How long we've been holding
    std::vector<double> mTimeHeld;

    std::unordered_set<dsp::noise_gate::Gain*> mGainListeners;

    double level_to_db(const DSP_SAMPLE db) { return 10.0 * log10(db); };

    bool gating{false};
    juce::Value isGatingValue {juce::var {false}};
};

#endif
