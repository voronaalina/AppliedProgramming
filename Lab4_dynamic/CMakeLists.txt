cmake_minimum_required(VERSION 3.20)
project(PolyLibProject LANGUAGES CXX)

add_library(PolyLib SHARED
    src/PolyLib.cpp
)

target_compile_definitions(PolyLib PRIVATE POLYLIB_EXPORTS)

target_include_directories(PolyLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(PolyLib PUBLIC cxx_std_20)