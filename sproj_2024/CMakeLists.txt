# FOR BUILDING SOURCE PROJECT

cmake_minimum_required(VERSION 3.22)

# Version is needed by JUCE.
project(sproj2024 VERSION 0.1.0)

# Adding a directory with the library/application name as a subfolder of the
# audio folder is a good practice. It helps avoid name clashes later on.
set(AUDIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/audio)
set(GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gui)
set(HELPERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/helpers)
set(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)

# Adds a plugin target (that's basically what the Projucer does).
#juce_add_plugin(${PROJECT_NAME}
#        COMPANY_NAME MyCompany # change this
#        IS_SYNTH FALSE # may change this
#        NEEDS_MIDI_INPUT FALSE # may change this
#        NEEDS_MIDI_OUTPUT FALSE # may change this
#        PLUGIN_MANUFACTURER_CODE ABCD # change this
#        PLUGIN_CODE EFGH # change this
#        FORMATS Standalone # may change this
#        PRODUCT_NAME "sproj2024" # change this
#        #COPY_PLUGIN_AFTER_BUILD TRUE # to run on DAW you must install plug-in with
#)

# Adds a standalone App
juce_add_gui_app(${PROJECT_NAME}
        # VERSION ...                       # Set this if the app version is different to the project version
        # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
        # ICON_SMALL ...
        # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
        # COMPANY_NAME ...                  # Specify the name of the app's author
        PRODUCT_NAME "Sproj2024")     # The name of the final executable, which can differ from the target name

#
#add_library(${PROJECT_NAME}_lib STATIC ${PROJECT_BINARY_DIR})


target_sources(${PROJECT_NAME}
        PRIVATE
        audio/AudioPlayer.h
        audio/AudioClock.h
        audio/MainAudio.h
        audio/Recorder.h
        audio/SPAudioProcessor.h
        audio/Track.h
        gui/ControlDeckGUI.h
        gui/DeckGUI.h
        gui/DeviceSelectionMenu.h
        gui/DummyClip.h
        gui/FreeDeckGUI.h
        gui/MainComponent.h
        gui/MainWindow.h
        gui/MenuComponent.h
        gui/MixDeckGUI.h
        gui/RulerDeckGUI.h
        gui/SPCommandManager.h
        gui/MainDeckHolder.h

        main/Main.cpp
        audio/MainAudio.cpp
        audio/SPAudioProcessor.cpp
        gui/MainComponent.cpp
        gui/MainWindow.cpp
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) #added

# Sets the audio directories of the plugin project.
target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/main
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers
)

# Links to all necessary dependencies. The present ones are recommended by JUCE.
# If you use one of the additional modules, like the DSP module, you need to specify it here.
target_link_libraries(${PROJECT_NAME}
        PRIVATE
            juce::juce_audio_devices
            juce::juce_audio_processors
            juce::juce_audio_utils
            juce::juce_gui_extra
            juce::juce_core
            juce::juce_dsp
            juce::juce_animation
            #jive::jive_layouts
            #jive::jive_style_sheets
            melatonin_inspector
            focusrite-e2e::focusrite-e2e
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags #link time optimization flags
            juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0

        PRIVATE
            JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_PRODUCT_NAME>"
            JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${PROJECT_NAME},JUCE_VERSION>"
)

# to generate the JuceHeader.h files (for compatibility with Projucer)
juce_generate_juce_header(${PROJECT_NAME})


# Enables all warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party!!!!!
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# In Visual Studio this command provides a nice grouping of gui files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)