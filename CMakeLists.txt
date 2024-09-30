#THIS IS A CMAKE FILE FOR 3rd PARTY SOFTWARE SETUP (like juce and CPM)


# Mandatory line, sets the minimum version of CMake that should be used with this repository.
# I specified 3.22 because I trust it. However, currently I have 3.26 installed on my machine.
# To verify your version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22)

# Sets a few variables, like PROJECT_NAME
project(sproj2024)

# Always use the newest C++ standard on green-field projects if possible.
# Currently, JUCE supports only C++ 20 cross-platform.
set(CMAKE_CXX_STANDARD 20)

# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/lib)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
CPMAddPackage(
        NAME JUCE
        GIT_TAG 8.0.1 #this is done because JUCE versions don't follow the "vX.X.X" convention that cpm has for versions
        VERSION 8.0.1 #(like v8.0.1 instead of just 8.0.1)
        GITHUB_REPOSITORY juce-framework/JUCE
        SOURCE_DIR ${LIB_DIR}/juce
)

# Adds googletest.
CPMAddPackage(
        NAME GOOGLETEST
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.15.2 #technically not necessary?
        VERSION 1.15.2
        SOURCE_DIR ${LIB_DIR}/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

# This command allows running tests from the "build" folder (the one where CMake generates the project to).
enable_testing()

# Adds all the targets configured in the "plugin_test_cmake" folder.
add_subdirectory(sproj_2024)

# Adds all the targets configured in the "test" folder.
#add_subdirectory(test)