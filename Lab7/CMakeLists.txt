cmake_minimum_required(VERSION 3.21)
project(Lab7 LANGUAGES CXX)

add_subdirectory(PolyLib)
add_subdirectory(App)
add_subdirectory(antipattern)

# Google Test
find_package(GTest CONFIG REQUIRED)
enable_testing()
add_subdirectory(tests)