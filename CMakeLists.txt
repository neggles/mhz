cmake_minimum_required(VERSION 3.25)

# Set policies
cmake_policy(SET CMP0042 NEW) # CMP0042: MACOSX_RPATH is enabled by default.
cmake_policy(SET CMP0053 NEW) # CMP0053: Simplify variable reference and escape sequence evaluation.
cmake_policy(SET CMP0054 NEW) # CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0077 NEW) # CMP0077: option() honors normal variables.

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release"
        CACHE STRING "Set build type to Debug or Release (default Debug)" FORCE)
endif()

project(mhz
    VERSION 0.2.0
    DESCRIPTION "architecture-agnostic processor clock measurement utility"
    LANGUAGES C ASM
)

# Set up GNU conventions and standard FHS paths
include(GNUInstallDirs)

# checking for libraries, symbols, LTO support etc
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckIPOSupported)

# Check if _GNU_SOURCE is available.
if(NOT DEFINED _GNU_SOURCE)
    check_symbol_exists(__GNU_LIBRARY__ "features.h" _GNU_SOURCE)

    if(NOT _GNU_SOURCE)
        unset(_GNU_SOURCE CACHE)
        check_symbol_exists(_GNU_SOURCE "features.h" _GNU_SOURCE)
    endif()
endif()

if(_GNU_SOURCE)
    string(APPEND CMAKE_C_FLAGS " -D_GNU_SOURCE")
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Where to place compiled static libraries.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Where to place compiled shared libraries.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CACHE PATH "Where to place compiled executables.")

# enable generation of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    string(APPEND CMAKE_C_FLAGS " -Og")
else()
    string(APPEND CMAKE_C_FLAGS " -O3")
endif()

# set C standard options
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# gcc needs this to find system headers
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
endif()

# force char to unsigned
add_compile_options(-funsigned-char)

# Set compiler options
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_STATIC_LIBS "Build using static libraries" ON)

# Add project subdirectory
add_subdirectory(${PROJECT_NAME})
