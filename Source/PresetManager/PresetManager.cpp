#include "PresetManager.h"
#include "../FileComparator.h"

const juce::File PresetManager::defaultPresetDirectory{
    juce::File::getSpecialLocation(juce::File::SpecialLocationType::userHomeDirectory).getChildFile("Neural Amp Modeler").getChildFile("Presets")};

const juce::String PresetManager::presetExtension{"nampreset"};
const juce::String PresetManager::presetNameProperty{"presetName"};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
{
    if (!defaultPresetDirectory.exists())
    {
        const auto result = defaultPresetDirectory.createDirectory();
        if (result.failed())
        {
            DBG("Error creating Preset Directory: " + result.getErrorMessage());
            jassertfalse;
        }
    }

    apvts.state.addListener(this);
    currentPreset.referTo(apvts.state.getPropertyAsValue(presetNameProperty, nullptr));
}

PresetManager::~PresetManager() {}

void PresetManager::savePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    currentPreset.setValue(presetName);

    const auto stateXML = this->createXmlFromPluginState(apvts.copyState());

    DBG(stateXML->toString());

    const auto presetFile = defaultPresetDirectory.getChildFile(presetName + "." + presetExtension);
    if (!stateXML->writeTo(presetFile))
    {
        DBG("Error creating preset file: " + presetFile.getFullPathName());
        jassertfalse;
    }
}

std::unique_ptr<juce::XmlElement> PresetManager::createXmlFromPluginState (juce::ValueTree state)
{
    // Remove ignored apvts parameters
    for (int i = state.getNumChildren(); --i >= 0;)
    {
        auto child = state.getChild(i);
        if (ignoredParams.contains(juce::String(child["id"].toString())))
            state.removeChild(i, nullptr);
    }

    auto stateXML = state.createXml();

    // Remove plugin state attributes
    for (const auto& attribute : ignoredAttributes)
        stateXML->removeAttribute(attribute);

    auto search_paths = stateXML->getChildByName("search_paths");
    stateXML->removeChildElement(search_paths, true);

    return stateXML;
}

void PresetManager::deletePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    const auto presetFile = defaultPresetDirectory.getChildFile(presetName + "." + presetExtension);
    if (!presetFile.existsAsFile())
    {
        DBG("Preset File " + presetFile.getFullPathName() + " does not exist");
        jassertfalse;
        return;
    }
    if (!presetFile.deleteFile())
    {
        DBG("Preset File " + presetFile.getFullPathName() + " could not be deleted");
        jassertfalse;
        return;
    }

    currentPreset.setValue("");
}

void PresetManager::loadPreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    const auto presetFile = defaultPresetDirectory.getChildFile(presetName + "." + presetExtension);
    if (!presetFile.existsAsFile())
    {
        DBG("Preset File " + presetFile.getFullPathName() + " does not exist");
        jassertfalse;
        return;
    }

    juce::XmlDocument xmlDocument{presetFile};
    // const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

    auto state = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());
    
    for (auto param : ignoredParams)
    {
        juce::ValueTree child ("PARAM");
        child.setProperty("id", param, nullptr);
        child.setProperty("value", juce::String(*apvts.getRawParameterValue(param)), nullptr);
        state.appendChild(child, nullptr);
    }

    // std::cout << state.toXmlString().toStdString() << std::endl;

    apvts.replaceState(state);
    currentPreset.setValue(presetName);
}

juce::StringArray PresetManager::getAllPresets() const
{
    juce::StringArray presets;
    auto fileArray = defaultPresetDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*" + presetExtension);

    FileComparator comparator;
    fileArray.sort(comparator);

    for (const auto& file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }

    return presets;
}

juce::String PresetManager::getCurrentPreset() const
{
    return currentPreset.toString();
}

int PresetManager::getNextPresetIndex()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;

    return nextIndex;
}

int PresetManager::getPreviousPresetIndex()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;

    return previousIndex;
}


bool PresetManager::loadNextPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return false;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;

    loadPreset(allPresets.getReference(nextIndex));
    return true;
}

bool PresetManager::loadPreviousPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return false;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;

    loadPreset(allPresets.getReference(previousIndex));
    return true;
}

void PresetManager::valueTreeRedirected(juce::ValueTree& treeChanged)
{
    currentPreset.referTo(treeChanged.getPropertyAsValue(presetNameProperty, nullptr));
}
