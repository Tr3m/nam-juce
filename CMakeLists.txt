cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "NEURAL_AMP_MODELER")
set(PLUGIN_VERSION 0.4.0)

project(${PROJECT_NAME} VERSION ${PLUGIN_VERSION})

include("${CMAKE_CURRENT_SOURCE_DIR}/CMake/helpers.cmake")
init_submodules()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include_directories(SYSTEM /usr/local/include)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    #link_libraries(stdc++fs)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    message(FATAL_ERROR "Unrecognized Platform!")
endif()

add_definitions(-DNAM_SAMPLE_FLOAT)
add_definitions(-DDSP_SAMPLE_FLOAT)

set(NAM_DEPS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/Dependencies")
set(AUDIO_DSP_TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/Dependencies/AudioDSPTools")

add_subdirectory(Modules/NeuralAmpModelerCore/Dependencies/eigen)

file(GLOB NAM_SOURCES_TOP "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/NAM/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/NAM/*.c")
file(GLOB NAM_SOURCES_SUB "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/NAM/*/*.cpp")
set(NAM_SOURCES ${NAM_SOURCES_TOP} ${NAM_SOURCES_SUB})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_PREFIX_PATH)
    find_package(JUCE CONFIG REQUIRED)
else()
    init_juce()
    add_subdirectory(Modules/JUCE)
endif()

# include_directories(SYSTEM Modules/NeuralAmpModelerCore)
# include_directories(SYSTEM Modules/AudioDSPTools)
# add_subdirectory(Modules/NeuralAmpModelerCore/Dependencies/eigen)
# add_subdirectory(Modules/json)

juce_add_plugin(${PROJECT_NAME}
    VERSION ${PLUGIN_VERSION}
    ICON_BIG "Assets/ICON.png"
    ICON_SMALL "Assets/ICON.png"
    COMPANY_NAME "Manos Labrakis"
    # IS_SYNTH TRUE/FALSE                 
    NEEDS_MIDI_INPUT TRUE           
    # NEEDS_MIDI_OUTPUT TRUE/FALSE        
    # IS_MIDI_EFFECT TRUE/FALSE         
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE  
    # PLUGIN_MANUFACTURER_CODE Juce             
    PLUGIN_CODE NMj0
    FORMATS AU VST3 Standalone                  
    BUNDLE_ID "com.ManosLabrakis.NAM_JUCE"
    PRODUCT_NAME "Neural Amp Modeler"
    JucePlugin_Manufacturer "Manos Labrakis"
    )        

add_library(nam_core STATIC ${NAM_SOURCES})

set_target_properties(nam_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(nam_core PUBLIC
    Modules/NeuralAmpModelerCore
    Modules/NeuralAmpModelerCore/NAM
    Modules/NeuralAmpModelerCore/Dependencies/eigen
    Modules/NeuralAmpModelerCore/Dependencies/nlohmann
)

target_sources(${PROJECT_NAME}
PRIVATE
    Modules/ff_meters/ff_meters.h
    Modules/ff_meters/ff_meters.cpp
    # Modules/NeuralAmpModelerCore/NAM/activations.cpp
    # Modules/NeuralAmpModelerCore/NAM/activations.h
    # Modules/NeuralAmpModelerCore/NAM/convnet.cpp
    # Modules/NeuralAmpModelerCore/NAM/convnet.h
    # Modules/NeuralAmpModelerCore/NAM/dsp.cpp
    # Modules/NeuralAmpModelerCore/NAM/dsp.h
    # Modules/NeuralAmpModelerCore/NAM/get_dsp.cpp
    # Modules/NeuralAmpModelerCore/NAM/lstm.cpp
    # Modules/NeuralAmpModelerCore/NAM/lstm.h
    # Modules/NeuralAmpModelerCore/NAM/util.cpp
    # Modules/NeuralAmpModelerCore/NAM/util.h
    # Modules/NeuralAmpModelerCore/NAM/version.h
    # Modules/NeuralAmpModelerCore/NAM/wavenet.cpp
    # Modules/NeuralAmpModelerCore/NAM/wavenet.h
    Modules/AudioDSPTools/dsp/dsp.cpp
    Modules/AudioDSPTools/dsp/dsp.h
    Modules/AudioDSPTools/dsp/ImpulseResponse.cpp
    Modules/AudioDSPTools/dsp/ImpulseResponse.h
    Modules/AudioDSPTools/dsp/NoiseGate.cpp
    Modules/AudioDSPTools/dsp/NoiseGate.h
    Modules/AudioDSPTools/dsp/RecursiveLinearFilter.cpp
    Modules/AudioDSPTools/dsp/RecursiveLinearFilter.h
    Modules/AudioDSPTools/dsp/Resample.h
    Modules/AudioDSPTools/dsp/ResamplingContainer
    Modules/AudioDSPTools/dsp/version.h
    Modules/AudioDSPTools/dsp/wav.cpp
    Modules/AudioDSPTools/dsp/wav.h    
)

juce_generate_juce_header(${PROJECT_NAME})

add_subdirectory(Assets)
add_subdirectory(Source)
include_directories(Source)

juce_add_module("Modules/melatonin_blur")

option(NAM_ENABLE_A2_FAST "Build the A2 fast-path WaveNet" ON)
if(NAM_ENABLE_A2_FAST)
	add_compile_definitions(NAM_ENABLE_A2_FAST)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(amd64)|(AMD64)|(x86_64)")
	option(USE_NATIVE_ARCH "Enable architecture-specific optimizations" OFF)

	if (USE_NATIVE_ARCH)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64-v3")
		message("Enabling -march=x86-64-v3")
	endif (USE_NATIVE_ARCH)
endif ()

if (BETA_RELEASE)
    set(PLUG_VERSION "${PLUGIN_VERSION} BETA")
else()
    set(PLUG_VERSION "${PLUGIN_VERSION}")
endif()

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        #JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1
        PLUG_VERSION="${PLUG_VERSION}"
    )     

if(ASIO_PATH)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_ASIO=1)
        message(STATUS "Set ASIO flag")
        target_include_directories(${PROJECT_NAME} PUBLIC ${ASIO_PATH})
        message(STATUS "\nAdded ASIO SDK to target include directories\n")
    else()
        message(STATUS "\nASIO not supported on this OS, ignoring the ASIO Flag...\n")
    endif()
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC JUCE_JACK=1)
    message("-- JACK support enabled")
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        BinaryData
        juce::juce_audio_utils
        juce::juce_dsp
        melatonin_blur
        #nlohmann_json::nlohmann_json
        #Eigen3::Eigen
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(${PROJECT_NAME}
        PRIVATE
            -Wl,--whole-archive
            nam_core
            -Wl,--no-whole-archive
    )
else()
  target_link_libraries(${PROJECT_NAME}
    PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,nam_core>"
  )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(nam_core PUBLIC stdc++fs)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        Modules/NeuralAmpModelerCore/NAM
        Modules/NeuralAmpModelerCore/Dependencies/eigen
        Modules/NeuralAmpModelerCore/Dependencies/nlohmann
        Modules/AudioDSPTools/dsp
        # Modules/json/include
        # Modules/json/include/nlohmann
        Modules/
    #INTERFACE
        #$<TARGET_PROPERTY:juce_plugin_modules,INCLUDE_DIRECTORIES>
    )


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(${PROJECT_NAME} 
    PRIVATE 
        NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if (MSVC)
	target_compile_options(${PROJECT_NAME} 
    PRIVATE
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	target_compile_options(${PROJECT_NAME} 
    PRIVATE
		-w
		"$<$<CONFIG:DEBUG>:-Og;-ggdb>"
		"$<$<CONFIG:RELWITHDEBINFO>:-Ofast>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
