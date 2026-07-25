#ifndef __MIDI_HANDLER_H__
#define __MIDI_HANDLER_H__

#include "MidiUtils.h"
#include "Mappings.h"
#include "../PresetManager/PresetManager.h"

class MidiHandler 
{
public:
    MidiHandler(PresetManager& presetMgr, juce::Value& presetValue, std::function<void()>&& loadModelAndIrFuntion);
    ~MidiHandler();

    void loadConfig(const juce::File& configFile, juce::AudioProcessorValueTreeState& apvts);
    void loadPcConfig(juce::StringArray&);
    bool saveConfig(const juce::File& file);
    bool savePcConfig(const juce::StringArray&);
    void processMidiBuffer(juce::MidiBuffer& midiBuffer);

    const juce::File midiDirectory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::userHomeDirectory).getChildFile("Neural Amp Modeler").getChildFile("Midi")};
    const juce::File defaultMidiConfig {midiDirectory.getChildFile("DefaultMidi.xml")};

    const juce::File defaultConfigDirectory{juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory).getChildFile("NamJuce")};
    const juce::File pcMappingsFile {defaultConfigDirectory.getChildFile("PcMappings.xml")};

    std::vector<MidiMappingDisplay> getMappingsForDisplay() const;

    void addMapping(const juce::String& parameterID, int cc, int channel, juce::AudioProcessorValueTreeState& apvts);
    void removeMapping(uint32_t id);
    void setMappingCC(uint32_t id, int newCC);
    void setMappingChannel(uint32_t id, int channel);
    void setMappingParameter(uint32_t id, const juce::String& parameterID, juce::AudioProcessorValueTreeState& apvts);
    void clearMappings();

    enum MessageType 
    {
        ControlChange = 0,
        ProgramChange
    };

private:
    std::vector<ControlChangeMapping> mappings;
    std::array<std::vector<ControlChangeMapping*>, 128> ccLookup;
    uint32_t nextID = 1;
    juce::StringArray presetMappings;

    void populateLookupTable(juce::XmlElement* routings, juce::AudioProcessorValueTreeState& apvts);
    void rebuildLookUpTable();
    void clearLookupTable();
    ControlChangeMapping* findMapping(uint32_t id);

    void handleCC(const juce::MidiMessage& msg);
    void handlePC(const juce::MidiMessage& msg);
    
    int getMessageType(const juce::MidiMessage& msg);
    std::function<void()> presetChanged;

    PresetManager& presetManager;
    juce::Value& presetValue;

};

#endif // __MIDI_HANDLER_H__
