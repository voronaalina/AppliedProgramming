cmake_minimum_required(VERSION 3.20)
project(SFML_Lab5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)
add_executable(SFML_Lab5 main.cpp)

target_link_libraries(SFML_Lab5 PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)