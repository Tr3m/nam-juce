#include "TopBarComponent.h"

TopBarComponent::TopBarComponent(NamJUCEAudioProcessor& p, std::function<void()>&& updateFunction)
    : AudioProcessorEditor (&p), audioProcessor (p), parentUpdater(std::move(updateFunction)), pmc(p.getPresetManager(), [&]() { parentUpdater(); })
{
    lnf.setColour(juce::PopupMenu::backgroundColourId, Colours::grey.withAlpha(0.6f));

    addAndMakeVisible(&pmc);
    pmc.setColour(juce::Colours::transparentWhite, 0.0f);

    settingsButton.reset(new juce::ImageButton("SettingsButton"));
    addAndMakeVisible(settingsButton.get());
    settingsButton->setImages(false, true, true, settingsUnpushed, 1.0f, juce::Colours::transparentBlack, settingsUnpushed, 1.0f, juce::Colours::transparentBlack, settingsPushed, 1.0f, juce::Colours::transparentBlack, 0);
    settingsButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
    settingsButton->setTooltip("Settings");
    settingsButton->onClick = [this]
    {
        if(settingsDropdown->isPopupActive())
            settingsDropdown->hidePopup();
        else
            settingsDropdown->showPopup();
    };

    settingsDropdown.reset(new juce::ComboBox("Settings"));
    addAndMakeVisible(settingsDropdown.get());
    settingsDropdown->setVisible(false);
    if(JUCEApplication::isStandaloneApp())
        settingsDropdown->addItem (TRANS("Audio/Midi Settings..."), 1);
    settingsDropdown->addItem (TRANS("Get Models..."), 2);
    settingsDropdown->addItem (TRANS("Info"), 3);
    settingsDropdown->addListener(this);
    settingsDropdown->setLookAndFeel(&lnf);
}

TopBarComponent::~TopBarComponent()
{
    
}

void TopBarComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
}

void TopBarComponent::resized()
{
    pmc.setBounds(getWidth() / 2 - 105, 5, 260, 30); 
    settingsButton->setBounds(pmc.getX() - 30, pmc.getY() + 3, 25, 25);
    settingsDropdown->setBounds(settingsButton->getBounds());
}

void TopBarComponent::setBackgroundColour(juce::Colour colour)
{
    backgroundColour = colour;
    repaint();
}

void TopBarComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if(comboBoxThatHasChanged == settingsDropdown.get())
    {
        int selection = JUCEApplication::isStandaloneApp() ? comboBoxThatHasChanged->getSelectedItemIndex() : 
            comboBoxThatHasChanged->getSelectedItemIndex() + 1;

        switch(selection)
        {
        case DropdownOptions::AudioSettings:
            if(JUCEApplication::isStandaloneApp())
                juce::StandalonePluginHolder::getInstance()->showAudioSettingsDialog();
            break;
        case DropdownOptions::GetModels:
            modelsURL.launchInDefaultBrowser();
            break;
        case DropdownOptions::Info:
            openInfoWindow("NEURAL AMP MODELER\n\nVersion 0.3.2\n\nA JUCE implementation of the Neural Amp Modeler Plugin.");
            break;
        default:
            break;
        }
        
        //std::cout<<std::to_string(comboBoxThatHasChanged->getSelectedItemIndex())<<std::endl;
        settingsDropdown->setSelectedItemIndex(-1, juce::NotificationType::dontSendNotification);
        
    }
}

void TopBarComponent::openInfoWindow(juce::String m)
{
    juce::DialogWindow::LaunchOptions options;
    auto* label = new Label();
    label->setText (m, dontSendNotification);
    label->setColour (Label::textColourId, Colours::whitesmoke);
    label->setJustificationType(juce::Justification::centred);
    options.content.setOwned (label);

    Rectangle<int> area (0, 0, 300, 200);

    options.content->setSize (area.getWidth(), area.getHeight());

    options.dialogTitle                   = "Info";
    options.dialogBackgroundColour        = juce::Colours::darkgrey;
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = true;
    options.resizable                     = false;

    dialogWindow = options.launchAsync();

    if (dialogWindow != nullptr)
        dialogWindow->centreWithSize (300, 200);
}