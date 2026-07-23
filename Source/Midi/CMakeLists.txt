target_sources(${PROJECT_NAME}
PRIVATE
    MidiHandler.cpp
    MidiHandler.h
    MidiUtils.h
    Mappings.h
)

add_subdirectory(MidiMappingsComponent)
include_directories(MidiMappingsComponent)
