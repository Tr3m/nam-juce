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

#endif // __MAPPINGS_H__
