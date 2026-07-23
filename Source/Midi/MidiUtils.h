#ifndef __MIDI_UTILS_H__
#define __MIDI_UTILS_H__

#include <JuceHeader.h>

struct MidiUtils 
{

    static inline void checkDefaultConfig(const juce::File& midiDirectory)
    {
        if (!midiDirectory.exists())
        {
            DBG("Creating MIDI Directory...");
            const auto result = midiDirectory.createDirectory();
            if (result.failed())
            {
                DBG("Error creating Preset Directory: " + result.getErrorMessage());
                jassertfalse;
            }
        }

        juce::File defaultMidiConfig = midiDirectory.getChildFile("DefaultMidi.xml");
        
        if (!defaultMidiConfig.existsAsFile())
        {
            DBG("Creating default MIDI config...");
            createMidiConfig(defaultMidiConfig, nullptr);
        }
    };


    static inline bool createMidiConfig(const juce::File& midiFile, juce::ValueTree* config)
    {
        juce::ValueTree rootNode("MidiConfig");
        juce::ValueTree routings("Routings");

        if (config != nullptr)
            routings.appendChild(*config, nullptr);

        rootNode.appendChild(routings, nullptr);

        auto xml = rootNode.createXml();

        if (xml == nullptr)
            return false;

        return xml->writeTo(midiFile);
    }

};

#endif // __MIDI_UTILS_H__
