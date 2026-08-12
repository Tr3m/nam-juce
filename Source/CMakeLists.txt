target_sources(${PROJECT_NAME}
PRIVATE
    ResamplingNAM.h
    NeuralAmpModeler.cpp
    NeuralAmpModeler.h
    StatusedTrigger.cpp
    StatusedTrigger.h
    ToneStack.cpp
    ToneStack.h
    NamEditor.h
    NamEditor.cpp
    PluginEditor.cpp
    PluginEditor.h
    PluginProcessor.cpp
    PluginProcessor.h
    AssetManager.h
    TopBarComponent.cpp
    TopBarComponent.h
    DoublerProcessor.h
    architecture.hpp
    FileComparator.h
    IrProcessor.cpp
    IrProcessor.h
    LedComponent.h
)

add_subdirectory(EQ)
include_directories(EQ)

add_subdirectory(CustomStandaloneWindow)
include_directories(CustomStandaloneWindow)

add_subdirectory(PresetManager)
include_directories(PresetManager)

add_subdirectory(Midi)
include_directories(Midi)

add_subdirectory(LookAndFeel)
include_directories(LookAndFeel)

add_subdirectory(Preferences)
include_directories(Preferences)
