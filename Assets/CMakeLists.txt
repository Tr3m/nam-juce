file(GLOB SRC_FILES    
    "Buttons/*.png"
    "EQ/*.png"
    "PresetManagerAssets/*.png"
)

juce_add_binary_data(BinaryData SOURCES
    knob.png
    knob_strip.png
    background.png
    buttonpushed.png
    buttonunpushed.png
    screens.png 
    knob_minimal.png
    led_off.png
    led_on.png
    slim_thumb_off.png
    slim_thumb_on.png
    Mohave-Bold.ttf
    Mohave-Regular.ttf
    ${SRC_FILES}
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
