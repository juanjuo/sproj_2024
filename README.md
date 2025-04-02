## Overview
This project is a **DAW-like application for live looping**, designed to provide intuitive control over clip rearrangement and musical structure in real time. Unlike traditional DAWs, this application prioritizes flexibility in live performance, allowing musicians to manipulate loops dynamically.

Built entirely using the [JUCE](https://github.com/juce-framework/JUCE) framework, it leverages JUCE’s powerful audio engine and GUI capabilities

## Features
✅ add features

## Building instructions

Uses CMake as the build system, with that you can create IDE specific project file for development; here's an example of how you would do that for Xcode.

This will create a project file for Xcode. (make sure you have Cmake version 3.22 or above)

### Linux Requirements

This could be true for MacOS as well, but on Linux distros the following dependences are required:

```bash
$ sudo apt install libwebkit2gtk-4.0-dev libasound2-dev libcurlpp-dev
```

### Step-by-step Installation

First you will need to copy the repository into a local folder, then call:

```bash
$ mkdir build

$ cd build

$ ./build_tools/build

$ cmake --build build
```

### Testing

After building the project, you can move to the `build` directory and run `ctest`.
Alternatively, you can use the build_tool `test`.

Google Test can be references [here](https://google.github.io/googletest/quickstart-cmake.html)

## Licensing Information
