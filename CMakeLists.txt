cmake_minimum_required(VERSION 3.22)

project(RS40 VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory — you need to clone JUCE and set this path
# git clone https://github.com/juce-framework/JUCE.git
add_subdirectory(JUCE)

# Plugin target
juce_add_plugin(RS40
    COMPANY_NAME "Random Samples"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE RnSm
    PLUGIN_CODE Rs40
    FORMATS VST3 AU Standalone        # Add AAX if you have the SDK
    BUNDLE_ID "com.randomsamples.rs40"
    PRODUCT_NAME "RS-40"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "RS-40 needs audio input access to process your instrument signal."
)

# Source files
target_sources(RS40
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/RS40LookAndFeel.h
        Source/DSP/RS40Channel.cpp
        Source/DSP/RS40Channel.h
        Source/DSP/InputStage.cpp
        Source/DSP/InputStage.h
        Source/DSP/EQSection.cpp
        Source/DSP/EQSection.h
        Source/DSP/DiodeClipper.cpp
        Source/DSP/DiodeClipper.h
        Source/DSP/OutputStage.cpp
        Source/DSP/OutputStage.h
        Source/DSP/RC4558Model.cpp
        Source/DSP/RC4558Model.h
        Source/DSP/Oversampling.h
        Source/DSP/BiquadFilter.h
)

# Embedded font resources
juce_add_binary_data(RS40_BinaryData
    SOURCES
        Resources/IBMPlexMono-Bold.ttf
)

# JUCE module dependencies
target_compile_definitions(RS40
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RS40
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_dsp
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        RS40_BinaryData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
