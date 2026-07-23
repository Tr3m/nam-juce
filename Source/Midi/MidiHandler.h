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
    void processMidiBuffer(juce::MidiBuffer& midiBuffer);

    const juce::File midiDirectory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::userHomeDirectory).getChildFile("Neural Amp Modeler").getChildFile("Midi")};
    const juce::File defaultMidiConfig {midiDirectory.getChildFile("DefaultMidi.xml")};

    std::vector<MidiMappingDisplay> getMappingsForDisplay() const;

    enum MessageType 
    {
        ControlChange = 0,
        ProgramChange
    };

private:
    std::array<std::vector<ControlChangeMapping>, 128> ccMappings;

    void populateLookupTable(juce::XmlElement* routings, juce::AudioProcessorValueTreeState& apvts);
    void clearLookupTable();

    void handleCC(const juce::MidiMessage& msg);
    
    int getMessageType(const juce::MidiMessage& msg);

};

#endif // __MIDI_HANDLER_H__
