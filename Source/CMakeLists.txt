target_sources(NAM
PRIVATE
    NeuralAmpModeler.cpp
    NeuralAmpModeler.h
    NamEditor.h
    NamEditor.cpp
    PluginEditor.cpp
    PluginEditor.h
    PluginProcessor.cpp
    PluginProcessor.h
    MyLookAndFeel.h
    AssetManager.h
    EqEditor.cpp
    EqEditor.h
    EqProcessor.cpp
    EqProcessor.h
    TopBarComponent.cpp
    TopBarComponent.h
    WebComponent.h
    architecture.hpp
)

add_subdirectory(NeuralAmpModelerCore)
include_directories(NeuralAmpModelerCore)

add_subdirectory(PresetManager)
include_directories(PresetManager)
