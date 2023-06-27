#include "PresetManager.h"

const juce::File PresetManager::defaultPresetDirectory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonApplicationDataDirectory)
    .getChildFile(ProjectInfo::companyName).getChildFile(ProjectInfo::projectName)};

const juce::String PresetManager::presetExtension{"preset"};
const juce::String PresetManager::presetNameProperty{ "presetName" };

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts):
    apvts(apvts)
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

PresetManager::~PresetManager()
{

}

void PresetManager::savePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
        return;

    currentPreset.setValue(presetName);
    const auto stateXML = apvts.copyState().createXml();
    const auto presetFile = defaultPresetDirectory.getChildFile(presetName + "." + presetExtension);
    if (!stateXML->writeTo(presetFile))
    {
        DBG("Error creating preset file: " + presetFile.getFullPathName());
        jassertfalse;
    }
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

    juce::XmlDocument xmlDocument { presetFile };
    const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

    apvts.replaceState(valueTreeToLoad);
    currentPreset.setValue(presetName);
}

juce::StringArray PresetManager::getAllPresets() const
{
    juce::StringArray presets;
    const auto fileArray = defaultPresetDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*" + presetExtension);

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

int PresetManager::loadNextPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
    
    loadPreset(allPresets.getReference(nextIndex)); //This will work on JUCE 6.1.5

    return nextIndex; 
}

int PresetManager::loadPreviousPreset()
{
    const auto allPresets = getAllPresets();
    if (allPresets.isEmpty())
        return -1;

    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;

    loadPreset(allPresets.getReference(previousIndex)); //This will work on JUCE 6.1.5

    return previousIndex;
}

void PresetManager::valueTreeRedirected(juce::ValueTree& treeChanged)
{
    currentPreset.referTo(treeChanged.getPropertyAsValue(presetNameProperty, nullptr));
}