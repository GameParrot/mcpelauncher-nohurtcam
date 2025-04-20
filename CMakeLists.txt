cmake_minimum_required(VERSION 3.21...3.31)

project(mcpelaunchernhc)

if(NOT CMAKE_ANDROID_ARCH_ABI MATCHES "^(x86_64|arm64-v8a)$")
    message(FATAL_ERROR "Unsupported ABI")
endif()

set(CMAKE_CXX_STANDARD 23)

set(CPM_SOURCE_CACHE ${PROJECT_SOURCE_DIR}/cpm-cache CACHE PATH "")
include(cmake/CPM.cmake)

add_library(mcpelaunchernhc SHARED
        src/main.cpp
        src/menu.hpp)

target_compile_options(mcpelaunchernhc PRIVATE -Wall -Wextra -Wconversion -Wpedantic)

CPMAddPackage("gh:BasedInc/libhat#69149a1cee5a0228492826bd71af1a32d73ed87e")
CPMAddPackage("gh:stephenberry/glaze@5.0.1")
target_link_libraries(mcpelaunchernhc PRIVATE libhat::libhat glaze::glaze)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(mcpelaunchernhc PRIVATE -flto)
    target_link_options(mcpelaunchernhc PRIVATE
            -flto -Wl,--version-script,${PROJECT_SOURCE_DIR}/map.txt -Wl,--no-undefined-version)
    set_target_properties(mcpelaunchernhc PROPERTIES
            LINK_DEPENDS ${PROJECT_SOURCE_DIR}/map.txt)
endif()

install(TARGETS mcpelaunchernhc LIBRARY DESTINATION mods)
