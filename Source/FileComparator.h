#pragma once

struct FileComparator
{
    static int compareElements (const juce::File& a, const juce::File& b)
    {
        return a.getFileName().compareIgnoreCase (b.getFileName());
    }
};
