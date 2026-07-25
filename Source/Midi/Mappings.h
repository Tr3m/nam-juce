#ifndef __MAPPINGS_H__
#define __MAPPINGS_H__

#include <JuceHeader.h>

struct ControlChangeMapping
{
    uint32_t uid;
    juce::String parameterID;
    juce::RangedAudioParameter* parameter;
    int ccNumber {1};
    int channel {0}; // Omni
    int type {1};
};

struct ProgramChangeMapping {};

struct MidiMappingDisplay
{
    uint32_t uid;
    juce::String parameterName;
    juce::String parameterID;
    int ccNumber;
    int channel;
    int type {1};
};

#endif // __MAPPINGS_H__
