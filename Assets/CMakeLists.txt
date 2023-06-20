file(GLOB SRC_FILES    
    "Buttons/*.png"
    "EQ/*.png"
)

juce_add_binary_data(BinaryData SOURCES
    knob.png
    background.png
    buttonPressed.png
    buttonUnpressed.png
    screens.png
    arrowexpand.png
    arrowcontract.png
    knob_minimal.png
    ${SRC_FILES}
)

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties(BinaryData PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)