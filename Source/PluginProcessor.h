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
#include "Preferences/Preferences.h"

#define NUM_INTERNAL_VALUES 7
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
    
    bool loadNamModel (juce::File modelToirLoad, bool suspendProcessing = false);
    bool loadNamModel (int modelIndex, bool suspendProcessing = false);
    
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
    std::array<juce::Value, 5>& getStateValuesArray() { return this->stateValues; };

    Preferences& getPreferences() { return this->preferences; };

    enum StateValues
    {
        EQ_BYPASS = 0,
        CAB_BYPASS,
        TONESTACK_BYPASS,
        NORMALIZE,
        PRESET_CHANGED
    };

    //==============================================================================
private:

    enum OutputFilters
    {
        LowCutF = 0,
        HighCutF
    };

    enum ValuesInternal
    {
        PRESET_CHANGED_VIA_MIDI = 0,
        PRESET_NEXT_CALLED,
        PRESET_PREV_CALLED,
        MODEL_NEXT_CALLED,
        MODEL_PREV_CALLED,
        IR_NEXT_CALLED,
        IR_PREV_CALLED
    };

    NeuralAmpModeler myNAM;
    double slimSize {1.0};

    bool isA2 {false};
    bool modelFound{false};

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
    
    std::array<juce::Value, 5> stateValues;

    MidiHandler midiHandler;

    void exportParameters(juce::StringArray& ids, juce::StringArray& names);
    juce::StringArray parameterIDs, parameterNames;

    void loadLastModelAndIr();

    juce::Value valuesInternal [NUM_INTERNAL_VALUES] {
        juce::Value(juce::var("")),     // Preset Midi State
        juce::Value(juce::var(0.0f)),   // Next Preset
        juce::Value(juce::var(0.0f)),   // Prev Prest
        juce::Value(juce::var(0.0f)),   // Next Model
        juce::Value(juce::var(0.0f)),   // Prev Model
        juce::Value(juce::var(0.0f)),   // Next IR 
        juce::Value(juce::var(0.0f))    // Prev IR 
    };

    std::unordered_map<std::string, juce::Value*> valuesDict
    {
        {"PRESET_CHANGED_INTERNAL_ID", &valuesInternal[ValuesInternal::PRESET_CHANGED_VIA_MIDI]},
        {"PRESET_NEXT_ID", &valuesInternal[ValuesInternal::PRESET_NEXT_CALLED]},
        {"PRESET_PREVIOUS_ID", &valuesInternal[ValuesInternal::PRESET_PREV_CALLED]},
        {"MODEL_NEXT_ID", &valuesInternal[ValuesInternal::MODEL_NEXT_CALLED]},
        {"MODEL_PREVIOUS_ID", &valuesInternal[ValuesInternal::MODEL_PREV_CALLED]},
        {"IR_NEXT_ID", &valuesInternal[ValuesInternal::IR_NEXT_CALLED]},
        {"IR_PREVIOUS_ID", &valuesInternal[ValuesInternal::IR_PREV_CALLED]}
    };

    void updateStateValues();
    void updateInternalStateValue(const std::string&);
    juce::StringArray ignoredParams {"PRESET_NEXT_ID", "PRESET_PREVIOUS_ID", "MODEL_NEXT_ID", "MODEL_PREVIOUS_ID", "IR_NEXT_ID", "IR_PREVIOUS_ID"};
    
    Preferences preferences;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamJUCEAudioProcessor)
};
