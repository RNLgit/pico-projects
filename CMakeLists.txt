cmake_minimum_required(VERSION 3.13...3.27)
include(pico_sdk_import.cmake)
#include(pico_extras_import_optional.cmake)
project(pico-projects C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(MY_PICO_PROJECTS_PATH ${PROJECT_SOURCE_DIR})

add_subdirectory(wifi_tests)
