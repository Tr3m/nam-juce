#include "MidiHandler.h"

MidiHandler::MidiHandler(PresetManager& presetMgr, juce::Value& presetValue)
    : presetManager(presetMgr), presetValue(presetValue) 
{
    MidiUtils::checkDefaultConfig(midiDirectory);
    MidiUtils::checkDefaultPcMappings();

    juce::StringArray dummy;
    this->loadPcConfig(dummy);
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

void MidiHandler::loadPcConfig(juce::StringArray& pcMappings)
{
    pcMappings.clear();
    DBG("Reading PC Mappings file");
    auto xml = juce::XmlDocument::parse(this->pcMappingsFile);

    if (xml == nullptr)
    {
        DBG("Couldn't parse PC MIDI config.");
        return;
    }

    auto* routings = xml->getChildByName("ProgramChangeRoutings");

    if (routings == nullptr)
        return;

    for (auto* entry = routings->getFirstChildElement(); entry != nullptr; entry = entry->getNextElement())
    {
        if (!entry->hasTagName("Entry"))
            continue;

        pcMappings.add(entry->getStringAttribute("preset"));
    }

    if (pcMappings.size() < 128)
    {
        DBG("Tampered config loaded!");

        for (int i = pcMappings.size(); i < 128; ++i)
            pcMappings.add("");
    }

    this->presetMappings.clear();
    this->presetMappings = pcMappings;

};

bool MidiHandler::saveConfig(const juce::File& file)
{
    auto xml = std::make_unique<juce::XmlElement>("MidiConfig");

    auto* routings = xml->createNewChildElement("Routings");

    for (const auto& mapping : mappings)
    {
        auto* entry = routings->createNewChildElement("Entry");

        entry->setAttribute("type", mapping.type);
        entry->setAttribute("parameter_id", mapping.parameterID);
        entry->setAttribute("cc_no", mapping.ccNumber);
        entry->setAttribute("channel", mapping.channel);
    }

    DBG("Saving MIDI Preset " + file.getFullPathName());

    return xml->writeTo(file);
}

bool MidiHandler::savePcConfig(const juce::StringArray& pcMappings)
{
    auto xml = std::make_unique<juce::XmlElement>("MidiConfig");
    auto* routings = xml->createNewChildElement("ProgramChangeRoutings");

    for (int i = 0; i < 128; ++i)
    {
        auto* entry = routings->createNewChildElement("Entry");
        entry->setAttribute("program", i);
        entry->setAttribute("preset", pcMappings[i]);
    }

    this->presetMappings.clear();
    this->presetMappings = pcMappings;

    return xml->writeTo(pcMappingsFile);
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
        case MessageType::ProgramChange:
            handlePC(msg);
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
        
        auto entryType = entry->getIntAttribute("type");
        auto parameterID = entry->getStringAttribute("parameter_id");
        auto cc = entry->getIntAttribute("cc_no");
        auto channel = entry->getIntAttribute("channel", 0);
        juce::RangedAudioParameter* parameter;

        if (cc < 0 || cc > 127)
        {
            DBG("Invalid CC: " << cc);
            continue;
        }
        
        switch(entryType)
        {
        case EntryTypes::Parameter:

            // auto* parameter = apvts.getParameter(parameterID);
            parameter = apvts.getParameter(parameterID);

            if (parameter == nullptr)
            {
                DBG("Unknown parameter: " << parameterID);
                continue;
            }

            mappings.push_back({ nextID++, parameterID, parameter, cc, channel, entryType });
            break;

        case EntryTypes::Preset:
            mappings.push_back({ nextID++, parameterID, nullptr, cc, channel, entryType });
            break;
        }


        // mappings.push_back({ nextID++, parameterID, parameter, cc, channel });
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
        
        switch (mapping->type)
        {
        case EntryTypes::Parameter:
            mapping->parameter->setValueNotifyingHost(value);
            break;
        case EntryTypes::Preset:
            DBG("(MIDI) Loading preset " + mapping->parameterID);
            presetManager.loadPreset(mapping->parameterID); // Might be a good idea to move this outside the audio thread...
            presetValue.setValue(juce::var(!presetValue.getValue()));
            break;
        }
    }
}

void MidiHandler::handlePC(const juce::MidiMessage& msg)
{

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
        result.push_back({ mapping.uid, 
                mapping.type == EntryTypes::Parameter ? mapping.parameter->getName(32) : mapping.parameterID, 
                mapping.parameterID, mapping.ccNumber, mapping.channel, mapping.type });
    }

    return result;
}

void MidiHandler::addMapping(const juce::String& parameterID, int cc, int channel, juce::AudioProcessorValueTreeState& apvts, EntryTypes type)
{
    juce::RangedAudioParameter* parameter;

    if (type == EntryTypes::Parameter)
    {
        // auto* parameter = apvts.getParameter(parameterID);
        parameter = apvts.getParameter(parameterID);

        if (parameter == nullptr)
            return;
    }

    mappings.push_back({ nextID++, parameterID, parameter, cc, channel, type });

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

void MidiHandler::setMappingParameter(uint32_t id, const juce::String& parameterID, juce::AudioProcessorValueTreeState& apvts, EntryTypes type)
{
    auto* mapping = findMapping(id);

    if (mapping == nullptr)
        return;
    
    juce::RangedAudioParameter* parameter;
    
    if (type == EntryTypes::Parameter)
    {
        // auto* parameter = apvts.getParameter(parameterID);
        parameter = apvts.getParameter(parameterID);

        if (parameter == nullptr)
            return;
    }

    mapping->parameter = parameter;
    mapping->parameterID = parameterID;

    this->rebuildLookUpTable();
}

void MidiHandler::clearMappings()
{
   this->clearLookupTable(); 
   mappings.clear();
   this->nextID = 1;
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
