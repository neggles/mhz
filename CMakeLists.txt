cmake_minimum_required(VERSION 3.24)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug
        CACHE STRING "Set build type to Debug or Release (default Debug)" FORCE)
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

project(mhz
    VERSION 0.2.0
    DESCRIPTION "architecture-agnostic processor clock measurement utility"
    LANGUAGES C
)

# Set up GNU conventions and standard FHS paths
include(GNUInstallDirs)

# Ensure _GNU_SOURCE is available
include(CheckSymbolExists)

# Ensure IPO/LTO is available
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
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_STATIC_LIBS "Build using static libraries" ON)

# Add project subdirectory
add_subdirectory(${PROJECT_NAME})
