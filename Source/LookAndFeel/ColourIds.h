#ifndef __COLOUR_IDS_H__
#define __COLOUR_IDS_H__

namespace CoolButtons
{
    constexpr int colourBase = 0x3000000;

    struct TextButton
    {
        enum ColourIds
        {
            textGlowColourId = colourBase
        };
    };

    struct Slider
    {
        enum ColourIds
        {
            thumbGlowColourId = colourBase + 0x100,
            thumbLedOffColourId,
            rotarySliderColourId
        };
    };

} // namespace CoolButtons

#endif // __COLOUR_IDS_H__
