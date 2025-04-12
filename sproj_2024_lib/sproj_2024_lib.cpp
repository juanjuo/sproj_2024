//
// Created by Juan Diego on 4/10/25.
//

#ifdef SPROJ_2024_LIB_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of SPROJ cpp file"
#endif

#include "sproj_2024_lib.h"

#include "audio/MainAudio.cpp"
#include "audio/SPAudioProcessor.cpp"
#include "gui/MainComponent.cpp"
#include "gui/MainWindow.cpp"
#include "main/Main.cpp"