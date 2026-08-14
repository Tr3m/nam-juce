#ifndef __PREFIXED_TEXT_EDITOR_H__
#define __PREFIXED_TEXT_EDITOR_H__

#include <JuceHeader.h>

class PrefixedTextEditor : public juce::TextEditor
{
public:
    PrefixedTextEditor()
    {
        // leftIndent = getLeftIndent();
        // topIndent = getTopIndent();
        // setIndents (leftIndent, topIndent);
    }
    
    // void setText(const String& newText, bool isA2 = false)
    // {
    //     // updatePrefixIndent(isA2);
    //     juce::TextEditor::setText(newText);
    // }
    //
    // void setText(const String& newText, bool sendTextChangeMessage, bool isA2 = false)
    // {
    //     // updatePrefixIndent(isA2);
    //     juce::TextEditor::setText(newText, sendTextChangeMessage);
    // }

    void paint (juce::Graphics& g) override
    {
        juce::TextEditor::paint (g);
        
        if (isA2Model)
        {
            g.setColour (juce::Colours::green);
            juce::Font font = getFont();
            font.setStyleFlags(juce::Font::FontStyleFlags::bold);
            font.setHeight(12.0f);
            g.setFont (font);
            g.drawText ("A2", 5, 0, 20, getHeight(), juce::Justification::centredLeft);
        }
    }
 
    void clear()
    {
        juce::TextEditor().clear();

        if (isA2Model)
            setA2(false);
    }

    void setA2 (bool isA2)
    {
        isA2Model = isA2;

        if (! haveOriginalIndent)
        {
            leftIndent = getLeftIndent();
            topIndent  = getTopIndent();
            haveOriginalIndent = true;
        }

        setIndents (isA2 ? leftIndent + indentOffset : leftIndent, topIndent);
    }

private:
    int leftIndent {0}, topIndent {0};
    bool isA2Model {false};
    bool haveOriginalIndent {false};
    int indentOffset {20};

private:
};

    

#endif //__PREFIXED_TEXT_EDITOR_H__
