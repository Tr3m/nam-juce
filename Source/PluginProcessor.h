#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "NeuralAmpModeler.h"
#include "IrProcessor.h"
#include <ff_meters/ff_meters.h>
#include "EQ/EqProcessor.h"
#include "DoublerProcessor.h"
#include "PresetManager/PresetManager.h"
#include "Midi/MidiHandler.h"
//==============================================================================
/**
 */
class NamJUCEAudioProcessor : public juce::AudioProcessor, public juce::Value::Listener
#if JucePlugin_Enable_ARA
    ,
                              public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    NamJUCEAudioProcessor();
    ~NamJUCEAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources () override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor () override;
    bool hasEditor () const override;

    //==============================================================================
    const juce::String getName () const override;

    bool acceptsMidi () const override;
    bool producesMidi () const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds () const override;

    //==============================================================================
    int getNumPrograms () override;
    int getCurrentProgram () override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void valueChanged(juce::Value &) override;
    
    bool loadNamModel (juce::File modelToirLoad, bool suspendProcessing = true);
    bool loadNamModel (int modelIndex, bool suspendProcessing = true);
    
    // Somewhat inaccurate name since the modelLoaded
    // boolean is updated during staging rather than loading.
    // See NeuralAmpModeler.cpp loadModel().
    bool isModelLoaded() { return myNAM.isModelLoaded(); };

    void clearNAM ();

    double getSlimmableSize() { return myNAM.getSlimSize(); };
    void setSlimmableSize(double size);

    bool loadImpulseResponse (juce::File irToLoad, bool suspendProcessing = true);
    bool loadImpulseResponse (int index, bool suspendProcessing = true);
    bool getIrStatus ();
    void clearIR ();

    bool getTriggerStatus ();
    StatusedTrigger* getTrigger() { return myNAM.getTrigger(); };

    bool supportsDoublePrecisionProcessing () const override;

    const std::string getLastModelPath() { return lastModelPath; };
    const std::string getLastModelName() { return lastModelName; };
    const std::string getLastIrPath() { return lastIrPath; };
    const std::string getLastIrName() { return lastIrName; };
    const std::string getLastModelSearchDirectory() { return lastModelSerachDir; };
    const std::string getLastIrSearchDirectory() { return lastIrSerachDir; };

    foleys::LevelMeterSource& getMeterInSource() { return meterInSource; }
    foleys::LevelMeterSource& getMeterOutSource() { return meterOutSource; }

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters ();

    PresetManager& getPresetManager() { return presetManager; };

    bool eqModuleVisible{false};

    void loadFromPreset (juce::String modelPath, juce::String irPath);

    bool isA2Model() { return this->isA2; };

    juce::StringArray getDirectoryModelNames() { return directoryModelNames; };
    juce::StringArray getDirectoryIrNames() { return directoryIrNames; };

    void loadNextModel();
    void loadPreviousModel();

    void loadNextIR();
    void loadPreviousIR();

    MidiHandler& getMidiHandler() { return this->midiHandler; };

    const juce::StringArray getParameterIDs() { return this->parameterIDs; };
    const juce::StringArray getParameterNames() { return this->parameterNames; };

    const juce::StringArray getAllPresets() { return this->presetManager.getAllPresets(); };

    void updateDirectoryModels(const std::string& currentPath);
    void updateDirectoryIRs(const std::string& currentPath);

    juce::Value* getStateValue(int index);
    std::array<juce::Value, 7>& getStateValuesArray() { return this->stateValues; };

    enum StateValues
    {
        EQ_BYPASS = 0,
        CAB_BYPASS,
        TONESTACK_BYPASS,
        NORMALIZE,
        PRESET_CHANGED,
        MODEL_PARENT_CHANGED,
        IR_PARENT_CHANGED
    };

    //==============================================================================
private:

    enum OutputFilters
    {
        LowCutF = 0,
        HighCutF
    };

    NeuralAmpModeler myNAM;
    double slimSize {1.0};

    bool isA2 {false};

    IrProcessor cab;
    bool irFound{false};
    bool irLoaded{false};

    std::atomic<float>* filterCuttofs[2];

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highCut, lowCut;

    std::string lastModelPath = "null";
    std::string lastModelName = "null";

    std::string lastIrPath = "null";
    std::string lastIrName = "null";

    std::string lastModelSerachDir = "null";
    std::string lastIrSerachDir = "null";

    std::string previousModelDir {"null"}, previousIrDir {"null"};

    EqProcessor tenBandEq;
    Doubler doubler;

    bool supportsDouble{false};

    foleys::LevelMeterSource meterInSource;
    foleys::LevelMeterSource meterOutSource;

    juce::StringArray directoryModelNames, directoryModelPaths;
    int modelIndex {0};

    juce::StringArray directoryIrNames, directoryIrPaths;
    int irIndex {0};

    PresetManager presetManager;

    bool prepareCalled {false};
    
    std::array<juce::Value, 7> stateValues;

    MidiHandler midiHandler;

    void exportParameters(juce::StringArray& ids, juce::StringArray& names);
    juce::StringArray parameterIDs, parameterNames;

    void loadLastModelAndIr();
    juce::Value presetMidiChanged{juce::var{""}};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamJUCEAudioProcessor)
};
