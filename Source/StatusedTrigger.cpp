#include "StatusedTrigger.h"

StatusedTrigger::StatusedTrigger() : mParams(0.05, -60.0, 1.5, 0.002, 0.050, 0.050), mSampleRate(0) {}

DSP_SAMPLE** StatusedTrigger::Process(DSP_SAMPLE** inputs, const size_t numChannels, const size_t numFrames)
{
    this->_PrepareBuffers(numChannels, numFrames);

    // A bunch of numbers we'll use a few times.
    const double alpha = pow(0.5, 1.0 / (this->mParams.GetTime() * this->mSampleRate));
    const double beta = 1.0 - alpha;
    const double threshold = this->mParams.GetThreshold();
    const double dt = 1.0 / this->mSampleRate;
    const double maxHold = this->mParams.GetHoldTime();
    const double maxGainReduction = this->_GetMaxGainReduction();
    // Amount of open or close in a sample: rate times time
    const double dOpen = -this->_GetMaxGainReduction() / this->mParams.GetOpenTime() * dt; // >0
    const double dClose = this->_GetMaxGainReduction() / this->mParams.GetCloseTime() * dt; // <0

    // The main algorithm: compute the gain reduction
    for (auto c = 0; c < numChannels; c++)
    {
        for (auto s = 0; s < numFrames; s++)
        {
            this->mLevel[c] =
                std::clamp(alpha * this->mLevel[c] + beta * (inputs[c][s] * inputs[c][s]), dsp::noise_gate::MINIMUM_LOUDNESS_POWER, 1000.0);
            const double levelDB = level_to_db(this->mLevel[c]);
            if (this->mState[c] == StatusedTrigger::State::HOLDING)
            {
                this->mGainReductionDB[c][s] = 0.0;
                this->mLastGainReductionDB[c] = 0.0;
                if (levelDB < threshold)
                {
                    this->mTimeHeld[c] += dt;
                    if (this->mTimeHeld[c] >= maxHold)
                        this->mState[c] = StatusedTrigger::State::MOVING;
                }
                else
                {
                    this->mTimeHeld[c] = 0.0;
                }
            }
            else
            { // Moving
                const double targetGainReduction = this->_GetGainReduction(levelDB);
                if (targetGainReduction > this->mLastGainReductionDB[c])
                {
                    const double dGain = std::clamp(0.5 * (targetGainReduction - this->mLastGainReductionDB[c]), 0.0, dOpen);
                    this->mLastGainReductionDB[c] += dGain;
                    if (this->mLastGainReductionDB[c] >= 0.0)
                    {
                        this->mLastGainReductionDB[c] = 0.0;
                        this->mState[c] = StatusedTrigger::State::HOLDING;
                        this->mTimeHeld[c] = 0.0;
                    }

                    if (levelDB > threshold)
                    {
                        this->gating = false;
                        this->isGatingValue.setValue(juce::var(false));
                    }
                }
                else if (targetGainReduction < this->mLastGainReductionDB[c])
                {
                    const double dGain = std::clamp(0.5 * (targetGainReduction - this->mLastGainReductionDB[c]), dClose, 0.0);
                    this->mLastGainReductionDB[c] += dGain;
                    if (this->mLastGainReductionDB[c] < maxGainReduction)
                    {
                        this->mLastGainReductionDB[c] = maxGainReduction;
                    }

                    gating = true;
                    this->isGatingValue.setValue(juce::var(true));
                }
                this->mGainReductionDB[c][s] = this->mLastGainReductionDB[c];
            }
        }
    }

    // Share the results with gain objects that are listening to this trigger:
    for (auto gain = this->mGainListeners.begin(); gain != this->mGainListeners.end(); ++gain)
        (*gain)->SetGainReductionDB(this->mGainReductionDB);

    // Copy input to output
    for (auto c = 0; c < numChannels; c++)
        memcpy(this->mOutputs[c].data(), inputs[c], numFrames * sizeof(DSP_SAMPLE));
    return this->_GetPointers();
}

void StatusedTrigger::_PrepareBuffers(const size_t numChannels, const size_t numFrames)
{
    const size_t oldChannels = this->_GetNumChannels();
    const size_t oldFrames = this->_GetNumFrames();
    this->DSP::_PrepareBuffers(numChannels, numFrames);

    const bool updateChannels = numChannels != oldChannels;
    const bool updateFrames = updateChannels || numFrames != oldFrames;

    if (updateChannels || updateFrames)
    {
        const double maxGainReduction = this->_GetMaxGainReduction();
        if (updateChannels)
        {
            this->mGainReductionDB.resize(numChannels);
            this->mLastGainReductionDB.resize(numChannels);
            std::fill(this->mLastGainReductionDB.begin(), this->mLastGainReductionDB.end(), maxGainReduction);
            this->mState.resize(numChannels);
            std::fill(this->mState.begin(), this->mState.end(), StatusedTrigger::State::MOVING);
            this->mLevel.resize(numChannels);
            std::fill(this->mLevel.begin(), this->mLevel.end(), dsp::noise_gate::MINIMUM_LOUDNESS_POWER);
            this->mTimeHeld.resize(numChannels);
            std::fill(this->mTimeHeld.begin(), this->mTimeHeld.end(), 0.0);
        }
        if (updateFrames)
        {
            for (auto i = 0; i < this->mGainReductionDB.size(); i++)
            {
                this->mGainReductionDB[i].resize(numFrames);
                std::fill(this->mGainReductionDB[i].begin(), this->mGainReductionDB[i].end(), maxGainReduction);
            }
        }
    }
}

void StatusedTrigger::addValueListener(juce::Value::Listener* l)
{
    this->isGatingValue.addListener(l);
}

void StatusedTrigger::removeValueListener(juce::Value::Listener* l)
{
    this->isGatingValue.removeListener(l);
}
