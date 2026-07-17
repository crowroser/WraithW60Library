cmake_minimum_required(VERSION 3.16)
project(WraithW60Library VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(WRAITH_BUILD_CLI "Build CLI tool" ON)
option(WRAITH_BUILD_OPENRGB "Build OpenRGB integration" OFF)
option(WRAITH_BUILD_TESTS "Build tests" OFF)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(HIDAPI QUIET hidapi)
endif()

if(NOT HIDAPI_FOUND)
    find_library(HIDAPI_LIBRARY hidapi)
    find_path(HIDAPI_INCLUDE_DIR hidapi.h PATH_SUFFIXES hidapi)
    if(HIDAPI_LIBRARY AND HIDAPI_INCLUDE_DIR)
        set(HIDAPI_LIBRARIES ${HIDAPI_LIBRARY})
        set(HIDAPI_INCLUDE_DIRS ${HIDAPI_INCLUDE_DIR})
        set(HIDAPI_FOUND TRUE)
    else()
        message(FATAL_ERROR "hidapi not found. Install libhidapi-dev (Linux) or use vcpkg (Windows).")
    endif()
endif()

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
