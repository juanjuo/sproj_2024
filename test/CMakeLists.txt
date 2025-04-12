cmake_minimum_required(VERSION 3.22)

project(sproj2024_test)

# Signals to CMake that we want to run tests from this directory.
enable_testing()

# Creates the test console application.
add_executable(${PROJECT_NAME}
        unit_tests/MainAudioTest.cpp
)

#Sets the necessary audio directories: ours, JUCE's, and googletest's.
target_include_directories(${PROJECT_NAME}
        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/helpers
        ${JUCE_SOURCE_DIR}/modules
)

juce_add_module(${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024_lib ALIAS_NAMESPACE sproj)

#target_compile_definitions(${PROJECT_NAME}
#        PUBLIC
#        JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
#        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
#        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
#)

#target_sources(${PROJECT_NAME}
#        PRIVATE
#
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/AudioPlayer.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/AudioClock.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/MainAudio.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/Recorder.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/Scheduler.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/SPAudioProcessor.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/Track.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/ControlDeckGUI.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/DeckGUI.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/DeviceSelectionMenu.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/DummyClip.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/FreeDeckGUI.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MainComponent.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MainWindow.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MenuComponent.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MixDeckGUI.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/RulerDeckGUI.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/SPCommandManager.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MainDeckHolder.h
#
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/main/Main.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/MainAudio.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/audio/SPAudioProcessor.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MainComponent.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../sproj_2024/gui/MainWindow.cpp
#)

#target_include_directories(${PROJECT_NAME}
#        PUBLIC
#        ${CMAKE_CURRENT_SOURCE_DIR}/audio
#        ${CMAKE_CURRENT_SOURCE_DIR}/gui
#        ${CMAKE_CURRENT_SOURCE_DIR}/main
#        ${CMAKE_CURRENT_SOURCE_DIR}/helpers
#)

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        juce::juce_audio_devices
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_gui_extra
        juce::juce_core
        juce::juce_dsp
        juce::juce_animation
        GTest::gtest_main
        focusrite-e2e::focusrite-e2e
        sproj::sproj_2024_lib
)

# Adds googletest-specific CMake commands at our disposal.
include(GoogleTest)


# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif ()


# Add all tests defined with googletest to the CMake metadata so that these tests are run upon a call to ctest in the test projects' binary directory.
if (CMAKE_GENERATOR STREQUAL Xcode)
    # On macOS arm64, all binaries have to be signed before running. In local development, the linker adds an ad-hoc placeholder signature. In Xcode however, the ad-hoc signature is delayed until after the “Run Script” build phase, so the POST_BUILD command added by gtest_discover_tests cannot run. Thus, we need to delay test discovery until run time.
    # Source: https://discourse.cmake.org/t/googletest-crash-when-using-cmake-xcode-arm64/5766/8
    gtest_discover_tests(${PROJECT_NAME} DISCOVERY_MODE PRE_TEST)
else ()
    gtest_discover_tests(${PROJECT_NAME})
endif ()