cmake_minimum_required(VERSION 3.15)

project(NAM VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

find_package(JUCE CONFIG REQUIRED)

juce_add_plugin(NAM
    VERSION 1.0.0
    # ICON_BIG ...                            
    # ICON_SMALL ...
    COMPANY_NAME "Manos Labrakis"
    # IS_SYNTH TRUE/FALSE                 
    # NEEDS_MIDI_INPUT TRUE/FALSE           
    # NEEDS_MIDI_OUTPUT TRUE/FALSE        
    # IS_MIDI_EFFECT TRUE/FALSE         
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE  
    # PLUGIN_MANUFACTURER_CODE Juce             
    # PLUGIN_CODE MlS0                                                            
    FORMATS AU VST3 Standalone                  
    PRODUCT_NAME "Neural Amp Modeler"
    JucePlugin_Manufacturer "Manos Labrakis"
    )        
    

    juce_generate_juce_header(NAM)
    
    #add_subdirectory(Assets)

    add_subdirectory(Source)
    include_directories(Source)

    add_subdirectory(Modules/json)

    add_subdirectory(Modules/eigen)


target_compile_definitions(NAM
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)     

target_link_libraries(NAM
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        #BinaryData
        juce::juce_audio_utils
        juce::juce_dsp
        nlohmann_json::nlohmann_json
        Eigen3::Eigen
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

target_include_directories(NAM
    PUBLIC
        Modules/eigen
        Modules/json/include
        Modules/json/include/nlohmann
    #INTERFACE
        #$<TARGET_PROPERTY:juce_plugin_modules,INCLUDE_DIRECTORIES>
    )