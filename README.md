# LoopScheduler

![LoopScheduler_2025](https://drive.google.com/file/d/1yD4m955-eacfMoWef-sz0ICrgxzI5WdI/view?usp=share_link).

## Overview
This project is a **DAW-like application for live looping**, designed to provide intuitive control over clip rearrangement and musical structure in real time. Unlike traditional DAWs, this application prioritizes flexibility in live performance, allowing musicians to manipulate loops dynamically.

Built entirely using the [JUCE](https://github.com/juce-framework/JUCE) framework, it leverages JUCE’s powerful audio engine and GUI capabilities

## Building instructions

Uses CMake as the build system, with that you can create IDE specific project files for development; here's an example of how you would do that for Xcode.

First you will need to copy the repository into a local folder, then call:

```bash
$ mkdir build

$ cd build

$ cmake -G Xcode ..$
```

This will create a project file for Xcode. (make sure you have Cmake version 3.22 or above)

## Testing instructions

Uses [Focusrite/juce-end-to-end](https://github.com/FocusriteGroup/juce-end-to-end/tree/main) library. to build you must have npm installed, then use npm init on the root of the repository to create the package.json files, you might need to manually add the path for the testing files in the package.json file. then you can go to the test directory and run npm test to run the tests
