#include "MidiHandler.h"

MidiHandler::MidiHandler()
{
    MidiUtils::checkDefaultConfig(midiDirectory);
}

MidiHandler::~MidiHandler() {}

void MidiHandler::loadConfig(const juce::File& configFile, juce::AudioProcessorValueTreeState& apvts)
{
    juce::File config;

    if (!configFile.existsAsFile()) 
    {
        DBG("File " + configFile.getFileName() + " not found! Loading default config.");
        config = defaultMidiConfig;
    }
    else
    {
        config = configFile;
    }

    DBG("Loading MIDI config: " + config.getFullPathName());

    // Parse xml
    auto xml = juce::XmlDocument::parse(config);

    if (xml == nullptr)
    {
        DBG("Couldn't parse MIDI config.");
        return;
    }

    auto* routings = xml->getChildByName("Routings");

    if (routings == nullptr)
        return;
    
    this->populateLookupTable(routings, apvts);
}

void MidiHandler::processMidiBuffer(juce::MidiBuffer& midiBuffer)
{
    for (const auto& metadata : midiBuffer)
    {
        const auto& msg = metadata.getMessage();

        switch (this->getMessageType(msg))
        {
        case MessageType::ControlChange:
            handleCC(msg);
            break;
        default:
            break;
        }
    }
}

void MidiHandler::populateLookupTable(juce::XmlElement* routings, juce::AudioProcessorValueTreeState& apvts)
{
    this->clearLookupTable();
    int entries = 0;

    for (auto* entry = routings->getFirstChildElement(); entry != nullptr; entry = entry->getNextElement())
    {
        if (!entry->hasTagName("Entry"))
            continue;

        auto parameterID = entry->getStringAttribute("parameter_id");
        auto cc = entry->getIntAttribute("cc_no");
        auto channel = entry->getIntAttribute("channel", 0);

        if (cc < 0 || cc > 127)
        {
            DBG("Invalid CC: " << cc);
            continue;
        }

        auto* parameter = apvts.getParameter(parameterID);

        if (parameter == nullptr)
        {
            DBG("Unknown parameter: " << parameterID);
            continue;
        }

        ccMappings[cc].push_back({parameter, cc, channel});
        entries += 1;
    }

    DBG("Loaded " + juce::String(entries) + " MIDI mappings!");
}

void MidiHandler::clearLookupTable()
{
    for (auto& mappings : ccMappings)
        mappings.clear();
}

void MidiHandler::handleCC(const juce::MidiMessage& msg)
{
    auto cc = msg.getControllerNumber();

    float value = msg.getControllerValue() / 127.0f;

    for (const auto& mapping : ccMappings[cc])
    {
        if (mapping.channel != 0 && mapping.channel != msg.getChannel())
            continue;

        mapping.parameter->setValueNotifyingHost(value);
    }
}

int MidiHandler::getMessageType(const juce::MidiMessage& msg)
{
    if (msg.isController())
        return MessageType::ControlChange;
    else if (msg.isProgramChange())
        return MessageType::ProgramChange;
    else
        return -1;
}

std::vector<MidiMappingDisplay> MidiHandler::getMappingsForDisplay() const
{
    std::vector<MidiMappingDisplay> result;

    for (int cc = 0; cc < 128; ++cc)
        for (const auto& mapping : ccMappings[cc])
            result.push_back({mapping.parameter->getName(32), cc, mapping.channel});

    return result;
}
