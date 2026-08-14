#include "TopBarComponent.h"

TopBarComponent::TopBarComponent(NamJUCEAudioProcessor& p,
        std::function<void()>&& updateFunction,
        std::function<void(juce::String)>&& showSavePresetDialogFunction,
        std::function<void()>&& showMidiMappingsFunction,
        std::function<void()>&& updateColourScheme)
    : AudioProcessorEditor(&p), audioProcessor(p),
    pmc(p.getPresetManager(), std::move(updateFunction), std::move(showSavePresetDialogFunction), p.getPreferences()),
    showMidiMappings(std::move(showMidiMappingsFunction)),
    colourSchemeChanged(std::move(updateColourScheme))
{
    lnf.setColour(juce::PopupMenu::backgroundColourId, audioProcessor.getPreferences().popupMenuColour);
    lnf.setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, audioProcessor.getPreferences().popupMenuSelectionColour);
    lnf.setColour(juce::ComboBox::ColourIds::outlineColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(&pmc);
    pmc.setColour(juce::Colours::transparentWhite, 0.0f);

    settingsButton.reset(new juce::ImageButton("SettingsButton"));
    addAndMakeVisible(settingsButton.get());
    settingsButton->setImages(false, true, true, settingsUnpushed, 1.0f, juce::Colours::transparentBlack, settingsUnpushed, 1.0f,
                              juce::Colours::transparentBlack, settingsPushed, 1.0f, juce::Colours::transparentBlack, 0);
    settingsButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
    settingsButton->setTooltip("Settings");
    settingsButton->onClick = [this]
    {
        if (settingsDropdown->isPopupActive())
            settingsDropdown->hidePopup();
        else
            settingsDropdown->showPopup();
    };

    settingsDropdown.reset(new juce::ComboBox("Settings"));
    addAndMakeVisible(settingsDropdown.get());
    settingsDropdown->setVisible(false);
    if (JUCEApplication::isStandaloneApp())
        settingsDropdown->addItem(TRANS("Audio/Midi Settings..."), 1);
    settingsDropdown->addItem(TRANS("Preferences..."), 2);
    settingsDropdown->addItem(TRANS("Midi Mappings"), 3);
    settingsDropdown->addItem(TRANS("Get Models..."), 4);
    settingsDropdown->addItem(TRANS("Info"), 5);
    settingsDropdown->addListener(this);
    settingsDropdown->setLookAndFeel(&lnf);
}

TopBarComponent::~TopBarComponent() {}

void TopBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
}

void TopBarComponent::resized()
{
    pmc.setBounds(getWidth() / 2 - 105, 5, 280, 30);
    settingsButton->setBounds(pmc.getX() - 30, pmc.getY() + 3, 25, 25);
    settingsDropdown->setBounds(settingsButton->getBounds());
}

void TopBarComponent::setBackgroundColour(juce::Colour colour)
{
    backgroundColour = colour;
    this->repaint();
}

void TopBarComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == settingsDropdown.get())
    {
        int selection = JUCEApplication::isStandaloneApp() ? comboBoxThatHasChanged->getSelectedItemIndex() : comboBoxThatHasChanged->getSelectedItemIndex() + 1;

        switch (selection)
        {
            case DropdownOptions::AudioSettings:
                if (JUCEApplication::isStandaloneApp())
                    juce::StandalonePluginHolder::getInstance()->showAudioSettingsDialog();
                break;
            case DropdownOptions::Preferences:
                this->showPreferencesWindow();
                break;
            case DropdownOptions::MidiMappings:
                showMidiMappings();
                break;
            case DropdownOptions::GetModels: 
                modelsURL.launchInDefaultBrowser(); 
                break;
            case DropdownOptions::Info:
                showInfoWindow();
                break;
            default: break;
        }

        // std::cout<<std::to_string(comboBoxThatHasChanged->getSelectedItemIndex())<<std::endl;
        settingsDropdown->setSelectedItemIndex(-1, juce::NotificationType::dontSendNotification);
    }
}

void TopBarComponent::setMenuSelectedId(int id)
{
    if (this->settingsDropdown != nullptr && id > 0)
        settingsDropdown->setSelectedId(id, juce::NotificationType::sendNotification);
}

void TopBarComponent::showInfoWindow()
{
    SafePointer<DialogWindow> dialogWindow;
    juce::DialogWindow::LaunchOptions options;

    auto* infoComponent = new InfoComponent(audioProcessor.getPreferences());
    options.content.setOwned(infoComponent);

    juce::Rectangle<int> area(0, 0, 340, 300);

    options.content->setSize(area.getWidth(), area.getHeight());

    options.dialogTitle = "Info";
    options.dialogBackgroundColour = juce::Colour::fromString("FF2E2E2E");
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;

    dialogWindow = options.launchAsync();
    // dialogWindow->setResizable(true, false);
    dialogWindow->setResizeLimits(340, 300, 340, 300);

    if (dialogWindow != nullptr)
        dialogWindow->centreWithSize(340, 300);
}

void TopBarComponent::showPreferencesWindow()
{
    SafePointer<DialogWindow> dialogWindow;
    
    juce::DialogWindow::LaunchOptions options;
    auto* preferencesComponent = new PreferencesWindow(audioProcessor, [&]() { this->colourSchemeChanged(); }); 
    options.content.setOwned(preferencesComponent);

    juce::Rectangle<int> area(0, 0, 500, 500);

    options.content->setSize(area.getWidth(), area.getHeight() + 10);

    options.dialogTitle = "Preferences";
    options.dialogBackgroundColour = juce::Colour::fromString("FF2E2E2E");
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.escapeKeyTriggersCloseButton = true;

    dialogWindow = options.launchAsync();
    dialogWindow->setResizeLimits(500, 400, 600, 800);
    
    if (dialogWindow != nullptr)
        dialogWindow->centreWithSize(500, 400);
}
