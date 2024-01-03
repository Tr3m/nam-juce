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
        settingsDropdown->addItem (TRANS("Audio/Midi Settings"), 1);
    settingsDropdown->addItem (TRANS("Get Models"), 2);
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
            openBrowserWindow();
            break;
        case DropdownOptions::Info:
            std::cout<<"Info window pops up here"<<std::endl;
            break;
        default:
            break;
        }
        
        //std::cout<<std::to_string(comboBoxThatHasChanged->getSelectedItemIndex())<<std::endl;
        settingsDropdown->setSelectedItemIndex(-1, juce::NotificationType::dontSendNotification);
        
    }
}

void TopBarComponent::openBrowserWindow()
{
    auto* dw = new WebComponent ("Tone Hunt", juce::Colours::black, juce::DocumentWindow::closeButton);
    dw->centreWithSize (850, 650);
    dw->setUsingNativeTitleBar(true);
    dw->setVisible (true);
}