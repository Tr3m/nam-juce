#pragma once

#include <JuceHeader.h>
#include "NeuralAmpModeler.h"

//==============================================================================
/**
*/
class NamJUCEAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    NamJUCEAudioProcessor();
    ~NamJUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce:: MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadNamModel(juce::File modelToLoad);
    void loadImpulseResponse(juce::File irToLoad);

    bool supportsDoublePrecisionProcessing() const override;

    const std::string getLastModelPath();
    const std::string getLastModelName();
    const std::string getLastIrPath();
    const std::string getLastIrName();

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


private:
    //==============================================================================

    NeuralAmpModeler myNAM;
    
    juce::AudioBuffer<float> fpBuffer; //Floating Point Buffer
    juce::dsp::Convolution cab;
    bool irFound {false};

    std::string lastModelPath = "null";
    std::string lastModelName = "null";

    std::string lastIrPath = "null";
    std::string lastIrName = "null";

    bool supportsDouble{ true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamJUCEAudioProcessor)
};
