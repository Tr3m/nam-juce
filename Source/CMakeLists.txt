target_sources(NAM
PRIVATE
    NeuralAmpModeler.cpp
    NeuralAmpModeler.h
    PluginEditor.cpp
    PluginEditor.h
    PluginProcessor.cpp
    PluginProcessor.h
    MyLookAndFeel.h
    architecture.hpp
)

add_subdirectory(NeuralAmpModelerCore)
include_directories(NeuralAmpModelerCore)
