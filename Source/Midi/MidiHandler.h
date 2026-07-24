#ifndef __MIDI_HANDLER_H__
#define __MIDI_HANDLER_H__

#include "MidiUtils.h"
#include "Mappings.h"

class MidiHandler 
{
public:
    MidiHandler();
    ~MidiHandler();

    void loadConfig(const juce::File& configFile, juce::AudioProcessorValueTreeState& apvts);
    bool saveConfig(const juce::File& file);
    void processMidiBuffer(juce::MidiBuffer& midiBuffer);

    const juce::File midiDirectory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::userHomeDirectory).getChildFile("Neural Amp Modeler").getChildFile("Midi")};
    const juce::File defaultMidiConfig {midiDirectory.getChildFile("DefaultMidi.xml")};

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

    void populateLookupTable(juce::XmlElement* routings, juce::AudioProcessorValueTreeState& apvts);
    void rebuildLookUpTable();
    void clearLookupTable();
    ControlChangeMapping* findMapping(uint32_t id);

    void handleCC(const juce::MidiMessage& msg);
    
    int getMessageType(const juce::MidiMessage& msg);

};

#endif // __MIDI_HANDLER_H__
