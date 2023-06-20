#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

class AssetManager
{
public:

    AssetManager()
    {
        //TODO: Refactor this using getNamedResource()
        toggleButtonAssets[0][0] = juce::ImageFileFormat::loadFrom(BinaryData::_00_png, BinaryData::_00_pngSize); 
        toggleButtonAssets[0][1] = juce::ImageFileFormat::loadFrom(BinaryData::_01_png, BinaryData::_01_pngSize);
        toggleButtonAssets[0][2] = juce::ImageFileFormat::loadFrom(BinaryData::_02_png, BinaryData::_02_pngSize);
        toggleButtonAssets[0][3] = juce::ImageFileFormat::loadFrom(BinaryData::_03_png, BinaryData::_03_pngSize);
        toggleButtonAssets[0][4] = juce::ImageFileFormat::loadFrom(BinaryData::_04_png, BinaryData::_04_pngSize);
        toggleButtonAssets[0][5] = juce::ImageFileFormat::loadFrom(BinaryData::_05_png, BinaryData::_05_pngSize);
        toggleButtonAssets[0][6] = juce::ImageFileFormat::loadFrom(BinaryData::_06_png, BinaryData::_06_pngSize);
        toggleButtonAssets[0][7] = juce::ImageFileFormat::loadFrom(BinaryData::_07_png, BinaryData::_07_pngSize);

        toggleButtonAssets[1][0] = juce::ImageFileFormat::loadFrom(BinaryData::_10_png, BinaryData::_10_pngSize); 
        toggleButtonAssets[1][1] = juce::ImageFileFormat::loadFrom(BinaryData::_11_png, BinaryData::_11_pngSize);
        toggleButtonAssets[1][2] = juce::ImageFileFormat::loadFrom(BinaryData::_12_png, BinaryData::_12_pngSize);
        toggleButtonAssets[1][3] = juce::ImageFileFormat::loadFrom(BinaryData::_13_png, BinaryData::_13_pngSize);
        toggleButtonAssets[1][4] = juce::ImageFileFormat::loadFrom(BinaryData::_14_png, BinaryData::_14_pngSize);
        toggleButtonAssets[1][5] = juce::ImageFileFormat::loadFrom(BinaryData::_15_png, BinaryData::_15_pngSize);
        toggleButtonAssets[1][6] = juce::ImageFileFormat::loadFrom(BinaryData::_16_png, BinaryData::_16_pngSize);
        toggleButtonAssets[1][7] = juce::ImageFileFormat::loadFrom(BinaryData::_17_png, BinaryData::_17_pngSize);
    }

    ~AssetManager()
    {

    }

    juce::Image getBackground()
    {
        return background;
    }

    juce::Image getScreens()
    {
        return screens;
    }

    void setLoadButton(std::unique_ptr<juce::ImageButton>& button)
    {
        button->setImages(false, true, false, loadButtonUnpressed, 1.0, juce::Colours::transparentWhite, loadButtonUnpressed, 1.0, juce::Colours::transparentWhite, loadButtonPressed, 1.0, juce::Colours::transparentWhite, 0);
    }

    void setClearButton(std::unique_ptr<juce::ImageButton>& button)
    {
        button->setImages(false, true, false, clearButtonUnpressed, 1.0, juce::Colours::transparentWhite, clearButtonUnpressed, 1.0, juce::Colours::transparentWhite, clearButtonPressed, 1.0, juce::Colours::transparentWhite, 0);
    }

    void setToggleButton(std::unique_ptr<juce::ImageButton>& button, bool moduleState, int buttonIndex)
    {
        button->setImages(false, true, false, toggleButtonAssets[int(moduleState)][buttonIndex + 1], 1.0, juce::Colours::transparentWhite, toggleButtonAssets[int(moduleState)][buttonIndex + 1], 1.0, juce::Colours::transparentWhite, toggleButtonAssets[int(moduleState)][buttonIndex], 1.0, juce::Colours::transparentWhite, 0);
    }

    enum ToggleButtonStates
    {
        IR_PRESSED = 0,
        IR_UNPRESSED,
        NORM_PRESSED,
        NORM_UNPRESSED,
        TS_PRESSED,
        TS_UNPRESSED,
        EQ_PRESSED,
        EQ_UNPRESSED
    };

    enum Buttons
    {
        IR_BUTTON = 0,
        NORMALIZE_BUTTON = 2,
        TONESTACK_BUTTON = 4,
        EQ_BUTTON = 6
    };

private:

    juce::Image background = juce::ImageFileFormat::loadFrom(BinaryData::background_png, BinaryData::background_pngSize);
    juce::Image screens = juce::ImageFileFormat::loadFrom(BinaryData::screens_png, BinaryData::screens_pngSize); 

    //Load IR Button Assets
    juce::Image loadButtonPressed = juce::ImageFileFormat::loadFrom(BinaryData::loadButtonPushed_png, BinaryData::loadButtonPushed_pngSize); 
    juce::Image loadButtonUnpressed = juce::ImageFileFormat::loadFrom(BinaryData::loadButtonUnpushed_png, BinaryData::loadButtonUnpushed_pngSize);

    juce::Image clearButtonPressed = juce::ImageFileFormat::loadFrom(BinaryData::clearButtonPressed_png, BinaryData::clearButtonPressed_pngSize); 
    juce::Image clearButtonUnpressed = juce::ImageFileFormat::loadFrom(BinaryData::clearButtonUnpressed_png, BinaryData::clearButtonUnpressed_pngSize);

    //Generic Button
    juce::Image buttonPressed = juce::ImageFileFormat::loadFrom(BinaryData::buttonPressed_png, BinaryData::buttonPressed_pngSize); 
    juce::Image buttonUnpressed = juce::ImageFileFormat::loadFrom(BinaryData::buttonUnpressed_png, BinaryData::buttonUnpressed_pngSize); 

    //Toggle Buttons
    juce::Image toggleButtonAssets[2][8];
    
     
};