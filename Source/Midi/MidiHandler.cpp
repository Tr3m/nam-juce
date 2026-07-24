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
    this->rebuildLookUpTable();
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

        mappings.push_back({ nextID++, parameterID, parameter, cc, channel });
        entries += 1;
    }

    DBG("Loaded " + juce::String(entries) + " MIDI mappings!");
}

void MidiHandler::rebuildLookUpTable()
{
    this->clearLookupTable();
    
    for (auto& mapping : mappings)
        ccLookup[mapping.ccNumber].push_back(&mapping);
}

void MidiHandler::clearLookupTable()
{
    for (auto& bucket : ccLookup)
            bucket.clear();
}

void MidiHandler::handleCC(const juce::MidiMessage& msg)
{
    auto cc = msg.getControllerNumber();

    float value = msg.getControllerValue() / 127.0f;

    for (auto* mapping : ccLookup[cc])
    {
        if (mapping->channel != 0 && mapping->channel != msg.getChannel())
            continue;

        mapping->parameter->setValueNotifyingHost(value);
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

    for (const auto& mapping : mappings)
    {
        result.push_back({ mapping.uid, mapping.parameter->getName(32), mapping.parameterID, mapping.ccNumber, mapping.channel });
    }

    return result;
}

void MidiHandler::addMapping(const juce::String& parameterID, int cc, int channel, juce::AudioProcessorValueTreeState& apvts)
{
    auto* parameter = apvts.getParameter(parameterID);

    if (parameter == nullptr)
        return;

    mappings.push_back({ nextID++, parameterID, parameter, cc, channel });

    rebuildLookUpTable();

}

void MidiHandler::removeMapping(uint32_t id)
{
    DBG("Removing mapping " + juce::String(id));
    auto it = std::remove_if(mappings.begin(), mappings.end(),
            [id](const auto& mapping)
            {
                return mapping.uid == id;
            });

    mappings.erase(it, mappings.end());

    rebuildLookUpTable();
}


void MidiHandler::setMappingCC(uint32_t id, int newCC)
{
    auto* mapping = findMapping(id);

    if (mapping == nullptr)
        return;

    mapping->ccNumber = newCC;

    this->rebuildLookUpTable();
}


void MidiHandler::setMappingChannel(uint32_t id, int channel)
{
    auto* mapping = findMapping(id);

    if (mapping == nullptr)
        return;

    mapping->channel = channel;

    this->rebuildLookUpTable();
}

void MidiHandler::setMappingParameter(uint32_t id, const juce::String& parameterID, juce::AudioProcessorValueTreeState& apvts)
{
    auto* mapping = findMapping(id);

    if (mapping == nullptr)
        return;

    auto* parameter = apvts.getParameter(parameterID);

    if (parameter == nullptr)
        return;

    mapping->parameter = parameter;
    mapping->parameterID = parameterID;

    this->rebuildLookUpTable();
}

ControlChangeMapping* MidiHandler::findMapping(uint32_t id)
{
    auto it = std::find_if(mappings.begin(), mappings.end(),
            [id](const auto& mapping)
            {
                return mapping.uid == id;
            });

    if (it == mappings.end())
        return nullptr;

    return &(*it);
}
