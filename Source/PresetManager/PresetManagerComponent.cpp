#include "PresetManagerComponent.h"

PresetManagerComponent::PresetManagerComponent(PresetManager& pm, std::function<void()>&& updateFunction) :
    presetManager(pm), parentUpdater(std::move(updateFunction))
{   
    constructUI();
}

void PresetManagerComponent::constructUI()
{
    lnf.setColour(juce::PopupMenu::backgroundColourId, juce::Colours::grey.withAlpha(0.6f));
    lnf.setColour(juce::TextEditor::textColourId, juce::Colours::aqua);
    lnf.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.7f));

    //addAndMakeVisible(&presetName);
    presetName.setLookAndFeel(&lnf);
    presetName.setReadOnly(true);

    presetComboBox.clear(juce::dontSendNotification);
    addAndMakeVisible (&presetComboBox);
    presetComboBox.setEditableText (false);
    presetComboBox.setJustificationType (juce::Justification::centredLeft);
    presetComboBox.addListener(this);

    presetComboBox.setLookAndFeel(&lnf);
    presetComboBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.0f));
    presetComboBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack.withAlpha(0.0f));
    presetComboBox.setAlpha(0.9f);

    loadComboBox();

    addAndMakeVisible(&previousButton);
    addAndMakeVisible(&nextButton);

    nextButton.setImages(false, true, true, forwardUnpushed, 1.0f, juce::Colours::transparentBlack, forwardUnpushed, 1.0f, juce::Colours::transparentBlack, forwardPushed, 1.0f, juce::Colours::transparentBlack, 0);
    previousButton.setImages(false, true, true, backUnushed, 1.0f, juce::Colours::transparentBlack, backUnushed, 1.0f, juce::Colours::transparentBlack, backPushed, 1.0f, juce::Colours::transparentBlack, 0);

    nextButton.onClick = [this]
    {
        const auto index = presetManager.loadNextPreset();
        presetComboBox.setSelectedItemIndex(index, juce::sendNotification);        
    };

    previousButton.onClick = [this]
    {
        const auto index = presetManager.loadPreviousPreset();
        presetComboBox.setSelectedItemIndex(index, juce::sendNotification);
    };

    addAndMakeVisible(&saveButton);
    saveButton.setImages(false, true, true, saveUnpushed, 1.0f, juce::Colours::transparentBlack, saveUnpushed, 1.0f, juce::Colours::transparentBlack, savePushed, 1.0f, juce::Colours::transparentBlack, 0);
    saveButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    saveButton.setTooltip("Save Preset");
    saveButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Enter Preset Name",
            PresetManager::defaultPresetDirectory,
            "*." + PresetManager::presetExtension
        );
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
            {
                const auto resultFile = chooser.getResult();
                presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                loadComboBox();
            });            
    };
}

void PresetManagerComponent::loadComboBox()
{
    presetComboBox.clear(juce::dontSendNotification);
    
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetComboBox.addItemList(presetManager.getAllPresets(), 1);
    presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
}

void PresetManagerComponent::setColour(juce::Colour colourToUse, float alpha)
{
    barColour = colourToUse;
    barAlpha = alpha;
    repaint();
}

void PresetManagerComponent::setColour(juce::Colour colourToUse)
{
    barColour = colourToUse;
    barAlpha = 0.4f;
    repaint();
}

void PresetManagerComponent::paint(juce::Graphics& g)
{
    g.fillAll(barColour.withAlpha(barAlpha));
    g.setColour(juce::Colours::white.withAlpha(0.3f));    
}

void PresetManagerComponent::resized()
{    
    presetName.setBounds(getWidth() * 0.1 + 5, (getHeight() / 2) - 12, getWidth() * 0.58, getHeight() - 6);
    presetComboBox.setBounds(getWidth() * 0.1 + 5, (getHeight() / 2) - 13, getWidth() * 0.58, getHeight() - 6);
    previousButton.setBounds(presetName.getX() - 30, (getHeight() / 2) - 12, 25, 25);
    nextButton.setBounds(presetName.getX() + presetName.getWidth() + 5, (getHeight() / 2) - 12, 25, 25);
    saveButton.setBounds(nextButton.getX() + nextButton.getWidth() + 5, (getHeight() / 2) - 12, 25, 25);
    
}

void PresetManagerComponent::parameterChanged()
{
    
}

void PresetManagerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    presetName.setText(comboBoxThatHasChanged->getItemText(comboBoxThatHasChanged->getSelectedId() - 1));
    presetManager.loadPreset(comboBoxThatHasChanged->getItemText(comboBoxThatHasChanged->getSelectedItemIndex()));

    parentUpdater();
}
