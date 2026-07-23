#ifndef __MAPPINGS_H__
#define __MAPPINGS_H__

#include <JuceHeader.h>

struct ControlChangeMapping
{
    juce::RangedAudioParameter* parameter;
    int ccNumber {1};
    int channel {0}; // Omni
};

struct ProgramChangeMapping {};

struct MidiMappingDisplay
{
    juce::String parameterName;
    int ccNumber;
    int channel;
};

#endif // __MAPPINGS_H__
