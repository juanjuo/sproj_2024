//
// Created by Juan Diego on 4/10/25.
//

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 sproj_2024_lib
  vendor:             Juan Diego Mora
  version:            1.0.0
  name:               Sproj classes
  description:        Classes from Sproj
  website:            http://www.juce.com/juce
  license:            AGPLv3/Commercial
  minimumCppStandard: 17

  dependencies:       juce_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define SPROJ_2024_LIB_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_animation/juce_animation.h>

#include "gui/SPCommandManager.h"
#include "audio/AudioClock.h"
#include "audio/AudioPlayer.h"
#include "audio/SPAudioProcessor.h"
#include "audio/MainAudio.h"
#include "audio/Recorder.h"
#include "audio/Track.h"

#include "gui/ControlDeckGUI.h"
#include "gui/DeckGUI.h"
#include "gui/DeviceSelectionMenu.h"
#include "gui/DummyClip.h"
#include "gui/FreeDeckGUI.h"
#include "gui/MainComponent.h"
#include "gui/MainWindow.h"
#include "gui/MenuComponent.h"
#include "gui/MixDeckGUI.h"
#include "gui/RulerDeckGUI.h"
#include "gui/MainDeckHolder.h"

#include "helpers/helpers.h"