include(ExternalProject)
include(CMakePackageConfigHelpers)

# Submodule Init Check Function
function(check_submoduled_initialized MODULED_INITIALIZED_PARENT)

    set(MODULED_INITIALIZED 1)

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/NAM")
        if(${MODULED_INITIALIZED_PARENT} EQUAL 1)
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore\n")
        endif()
        set(MODULED_INITIALIZED 0)
    elseif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/Dependencies/eigen/Eigen/src")
        if(${MODULED_INITIALIZED_PARENT} EQUAL 1)
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore/Dependencies/eigen\n")
        endif()
            set(MODULED_INITIALIZED 0)
    endif()

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/AudioDSPTools/dsp")
        if(${MODULED_INITIALIZED_PARENT} EQUAL 1)
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/AudioDSPTools\n")
        endif()
            set(MODULED_INITIALIZED 0)
    endif()    

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/ff_meters/ff_meters.h")
        if(${MODULED_INITIALIZED_PARENT} EQUAL 1)
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/ff_meters\n")
        endif()
            set(MODULED_INITIALIZED 0)
    endif()

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/melatonin_blur/melatonin_blur.h")
        if(${MODULED_INITIALIZED_PARENT} EQUAL 1)
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/melatonin_blur\n")
        endif()
            set(MODULED_INITIALIZED 0)
    endif()

    if(${MODULED_INITIALIZED_PARENT} EQUAL 0)
        if (${MODULED_INITIALIZED} EQUAL 1)
            SET(${MODULED_INITIALIZED_PARENT} ${MODULED_INITIALIZED} PARENT_SCOPE)
        endif()
    else()
        if (${MODULED_INITIALIZED} EQUAL 1)
            message("\nSubmodules Initialized!\n")
        endif()
    endif()


endfunction(check_submoduled_initialized)

# Submodule Init Function
function(init_submodules)
    set(INIT_SUCCESS 0)
    check_submoduled_initialized(INIT_SUCCESS)

    if (${INIT_SUCCESS} EQUAL 0) # Check if submodule already initialized
        message("\nInitializing Git Submodules...\n")
        find_program(GIT_CMD git REQUIRED)
        execute_process(COMMAND "${GIT_CMD}" submodule update --init "${CMAKE_CURRENT_SOURCE_DIR}/Modules/AudioDSPTools")
        execute_process(COMMAND "${GIT_CMD}" submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/Modules/NeuralAmpModelerCore")
        execute_process(COMMAND "${GIT_CMD}" submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/Modules/ff_meters")
        execute_process(COMMAND "${GIT_CMD}" submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/Modules/melatonin_blur")

        set(FINAL_CHECK 1)
        check_submoduled_initialized(FINAL_CHECK)

    endif()
endfunction(init_submodules)

# JUCE Submodule Init Function
function(init_juce)

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/JUCE/modules") # Check if submodule already initialized
        message("\nInitializing JUCE submodule...\n")
        find_program(GIT_CMD git REQUIRED)
        execute_process(COMMAND "${GIT_CMD}" submodule update --init --recursive "${CMAKE_CURRENT_SOURCE_DIR}/Modules/JUCE")

        if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Modules/JUCE/modules")
            message("Failed to initialize ${CMAKE_CURRENT_SOURCE_DIR}/Modules/JUCE\n")
        else()
            message("\nJUCE submodule Initialized!\n")
        endif()

    endif()
endfunction(init_juce)
