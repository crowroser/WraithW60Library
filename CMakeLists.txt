cmake_minimum_required(VERSION 3.16)
project(WraithW60Library VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(WRAITH_BUILD_CLI "Build CLI tool" ON)
option(WRAITH_BUILD_OPENRGB "Build OpenRGB integration" OFF)
option(WRAITH_BUILD_TESTS "Build tests" OFF)

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(HIDAPI QUIET hidapi hidapi-hidraw)
endif()

# Fallback: find library and header manually
if(NOT HIDAPI_FOUND)
    find_library(HIDAPI_LIBRARY NAMES hidapi hidapi-hidraw hidapi-libusb
        HINTS /usr/lib /usr/lib64 /usr/local/lib /usr/lib/x86_64-linux-gnu)
    find_path(HIDAPI_INCLUDE_DIR NAMES hidapi.h
        HINTS /usr/include /usr/local/include /usr/include/hidapi
        PATH_SUFFIXES hidapi)
    if(HIDAPI_LIBRARY AND HIDAPI_INCLUDE_DIR)
        set(HIDAPI_LIBRARIES ${HIDAPI_LIBRARY})
        set(HIDAPI_INCLUDE_DIRS ${HIDAPI_INCLUDE_DIR})
        set(HIDAPI_FOUND TRUE)
    endif()
endif()

# Last resort: try to find just the library (header might be in system include)
if(NOT HIDAPI_FOUND)
    find_library(HIDAPI_LIBRARY NAMES hidapi)
    if(HIDAPI_LIBRARY)
        set(HIDAPI_LIBRARIES ${HIDAPI_LIBRARY})
        set(HIDAPI_INCLUDE_DIRS /usr/include)
        set(HIDAPI_FOUND TRUE)
    endif()
endif()

if(NOT HIDAPI_FOUND)
    message(FATAL_ERROR "hidapi not found!\n"
        "Install:\n"
        "  Ubuntu/Debian: sudo apt install libhidapi-dev\n"
        "  Fedora:        sudo dnf install hidapi-devel\n"
        "  Arch:          sudo pacman -S hidapi\n"
        "  Windows:       vcpkg install hidapi\n"
        "  macOS:         brew install hidapi\n"
        "  Or build from source: https://github.com/libusb/hidapi")
endif()

message(STATUS "Found hidapi: ${HIDAPI_LIBRARIES}")
message(STATUS "hidapi includes: ${HIDAPI_INCLUDE_DIRS}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${HIDAPI_INCLUDE_DIRS}
)

add_subdirectory(src/core)

if(WRAITH_BUILD_OPENRGB)
    add_subdirectory(src/openrgb)
endif()

if(WRAITH_BUILD_CLI)
    add_subdirectory(src/cli)
endif()

if(WRAITH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(DIRECTORY include/wraith DESTINATION include)