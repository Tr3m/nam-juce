#pragma once
#include <JuceHeader.h>
#include "MyLookAndFeel.h"

#ifdef _WIN64
const juce::File data_directory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::windowsLocalAppData)
    .getChildFile("Loudsample").getChildFile("Neural Amp Modeler")};

const juce::File dll_directory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::windowsLocalAppData)
    .getChildFile("Loudsample").getChildFile("Neural Amp Modeler").getChildFile("WebView2Loader.dll")};
#endif

class BrowserComponentWrapper : public WebBrowserComponent
{
public:

    BrowserComponentWrapper()
    #ifdef _WIN64
        : WebBrowserComponent(juce::WebBrowserComponent::Options().withBackend(juce::WebBrowserComponent::Options::Backend::webview2).withWinWebView2Options(
            juce::WebBrowserComponent::Options::WinWebView2()
            .withDLLLocation(dll_directory)
            .withUserDataFolder(data_directory)
            .withBuiltInErrorPageDisabled()
            .withStatusBarDisabled()))
    #endif
    {

    }

    bool pageAboutToLoad (const String& newURL) override
    {
        
        if(newURL.startsWith("https://tonehunt.org/") || newURL.startsWith("blob:https://tonehunt.org/"))
        {
            currentURL = newURL;
            //DBG("NEW PAGE: " + newURL);
        }
        else
        {
            currentURL = "https://tonehunt.org/all?filter=amp";
            goToURL(currentURL);
        }
        
        return true;
    };

    // This method gets called when the browser is requested to launch a new window
    void newWindowAttemptingToLoad (const String& newURL) override
    {
        // We'll just load the URL into the main window
        goToURL (newURL);
    }

    juce::String getCurrentURL()
    {
        return currentURL;
    };

private:
    juce::String currentURL {"https://tonehunt.org/all?filter=amp"};
};

//===============================================================================

class WebComponent : public juce::DocumentWindow, public juce::Timer
{
public:
    WebComponent(const String& name, Colour backgroundColour, int buttonsNeeded)
        : DocumentWindow (name, backgroundColour, buttonsNeeded)
    {
        lnf.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);

        setSize(600, 900);
        addAndMakeVisible(&webView);        
        webView.goToURL("https://tonehunt.org/all?filter=amp"); 
        webView.toBehind(&back);

        addAndMakeVisible(&back);
        back.setButtonText("<<");
        back.setLookAndFeel(&lnf);
        back.onClick = [this]{webView.goBack();};
        back.toFront(false);
        back.setAlwaysOnTop(true);
        
        addAndMakeVisible(&homeButton);
        homeButton.setButtonText("Home");
        homeButton.setLookAndFeel(&lnf);
        homeButton.onClick = [this]{webView.goToURL("https://tonehunt.org/all?filter=amp");};

        //startTimerHz(30);
    };

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
    };

    void closeButtonPressed()
    {
        delete this;
    }
    
    void resized() override
    {
        webView.setBounds(0, 28, getWidth(), getHeight() - 28);
        back.setBounds(5, 3, 26, 22);
        homeButton.setBounds(10 + 26, 3, 48, 22);
    };

    void timerCallback() override
    {
        //DBG(webView.getCurrentURL());
    };

private:
    BrowserComponentWrapper webView;
    juce::TextButton back, homeButton;
    ButtonLookAndFeel lnf;
};