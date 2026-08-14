#include "PresetManagerComponent.h"

PresetManagerComponent::PresetManagerComponent(PresetManager& pm, std::function<void()>&& updateFunction,
    std::function<void(const juce::String& currentPreset)>&& presetDialogFunction, Preferences& pref)
    : presetManager(pm), parentUpdater(std::move(updateFunction)), showPresetDialog(std::move(presetDialogFunction)), preferences(pref)
{
    constructUI();
}

void PresetManagerComponent::constructUI()
{
    cbLnf.setColour(juce::PopupMenu::backgroundColourId, preferences.popupMenuColour);
    cbLnf.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, preferences.popupMenuSelectionColour);
    cbLnf.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    cbLnf.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);

    lnf.setColour(juce::TextEditor::textColourId, juce::Colours::aqua);
    lnf.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.7f));

    // addAndMakeVisible(&presetName);
    presetName.setLookAndFeel(&lnf);
    presetName.setReadOnly(true);

    presetComboBox.clear(juce::dontSendNotification);
    addAndMakeVisible(&presetComboBox);
    presetComboBox.setEditableText(false);
    presetComboBox.setJustificationType(juce::Justification::centredLeft);
    presetComboBox.addListener(this);

    presetComboBox.setLookAndFeel(&cbLnf);
    presetComboBox.setAlpha(0.9f);

    loadComboBox();

    addAndMakeVisible(&previousButton);
    addAndMakeVisible(&nextButton);

    nextButton.setImages(false, true, true, forwardUnpushed, 1.0f, juce::Colours::transparentBlack, forwardUnpushed, 1.0f,
                         juce::Colours::transparentBlack, forwardPushed, 1.0f, juce::Colours::transparentBlack, 0);
    previousButton.setImages(false, true, true, backUnushed, 1.0f, juce::Colours::transparentBlack, backUnushed, 1.0f,
                             juce::Colours::transparentBlack, backPushed, 1.0f, juce::Colours::transparentBlack, 0);

    nextButton.onClick = [this]
    {
        const auto index = presetManager.getNextPresetIndex();
        presetComboBox.setSelectedItemIndex(index, juce::sendNotification);
        updateCurrentSelection();
    };

    previousButton.onClick = [this]
    {
        const auto index = presetManager.getPreviousPresetIndex();
        presetComboBox.setSelectedItemIndex(index, juce::sendNotification);
        updateCurrentSelection();
    };

    addAndMakeVisible(&saveButton);
    saveButton.setImages(false, true, true, saveUnpushed, 1.0f, juce::Colours::transparentBlack, saveUnpushed, 1.0f, juce::Colours::transparentBlack,
                         savePushed, 1.0f, juce::Colours::transparentBlack, 0);
    saveButton.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    saveButton.setTooltip("Save Preset");
    saveButton.onClick = [this]
    {
       showPresetDialog(presetComboBox.getItemText(presetComboBox.getSelectedItemIndex()));
    };

    addAndMakeVisible(&deleteButton);
    deleteButton.setImages(false, true, true, deleteUnpushed, 1.0f, juce::Colours::transparentBlack, deleteUnpushed, 1.0f, juce::Colours::transparentBlack, deletePushed, 1.0f, juce::Colours::transparentBlack, 0);
    deleteButton.setTooltip("Delete Selected Preset");

    deleteButton.onClick = [this] { this->deleteSelectedPreset(); };
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
    this->repaint();
}

void PresetManagerComponent::setColour(juce::Colour colourToUse)
{
    barColour = colourToUse;
    barAlpha = 0.4f;
    repaint();
}

void PresetManagerComponent::updateAfterMidiLoad()
{
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetComboBox.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
    updateCurrentSelection();
}

void PresetManagerComponent::paint(juce::Graphics& g)
{
    g.fillAll(barColour.withAlpha(barAlpha));
    g.setColour(juce::Colours::white.withAlpha(0.3f));
}

void PresetManagerComponent::resized()
{
    int textBoxX = 31;
    int textBoxWidth = 150;

    presetName.setBounds(textBoxX, (getHeight() / 2) - 12, textBoxWidth, getHeight() - 6);
    presetComboBox.setBounds(textBoxX, (getHeight() / 2) - 13, textBoxWidth, getHeight() - 6);

    // presetName.setBounds(getWidth() * 0.1 + 5, (getHeight() / 2) - 12, getWidth() * 0.58, getHeight() - 6);
    // presetComboBox.setBounds(getWidth() * 0.1 + 5, (getHeight() / 2) - 13, getWidth() * 0.58, getHeight() - 6);

    previousButton.setBounds(presetName.getX() - 30, (getHeight() / 2) - 12, 25, 25);
    nextButton.setBounds(presetName.getX() + presetName.getWidth() + 5, (getHeight() / 2) - 12, 25, 25);
    saveButton.setBounds(nextButton.getX() + nextButton.getWidth() + 5, (getHeight() / 2) - 12, 25, 25);
    deleteButton.setBounds(saveButton.getX() + saveButton.getWidth() + 5, saveButton.getY(), 25, 25);
}

void PresetManagerComponent::parameterChanged() {}

void PresetManagerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    presetName.setText(comboBoxThatHasChanged->getItemText(comboBoxThatHasChanged->getSelectedId() - 1));
    presetManager.loadPreset(comboBoxThatHasChanged->getItemText(comboBoxThatHasChanged->getSelectedItemIndex()));

    parentUpdater();
}

void PresetManagerComponent::updateCurrentSelection()
{
    currentSelection = presetComboBox.getSelectedId();
};

void PresetManagerComponent::deleteSelectedPreset()
{

    this->getLookAndFeel().setColour(juce::AlertWindow::backgroundColourId, juce::Colour::fromString("FF2E2E2E"));
    this->getLookAndFeel().setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
    this->getLookAndFeel().setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::snow);
    this->getLookAndFeel().setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::snow);

    auto pName = presetComboBox.getItemText(presetComboBox.getSelectedItemIndex());

    if (!pName.trim().isEmpty())
    {
        auto shouldDelete = juce::AlertWindow::showOkCancelBox (juce::MessageBoxIconType::QuestionIcon, "Delete Preset", 
            "Delete preset " + pName + "?", {}, {}, {}, nullptr);

        if(shouldDelete)
        {
            presetManager.deletePreset(pName);
            loadComboBox();
        }
    }
};
