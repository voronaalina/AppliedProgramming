cmake_minimum_required(VERSION 3.20)
project(Lab_4_2 LANGUAGES CXX)

add_executable(ImplicitApp main.cpp)

target_include_directories(ImplicitApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/InterpolationDynamic/include
)

target_link_libraries(ImplicitApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/InterpolationDynamic/lib/Interpolation.lib
)

add_custom_command(TARGET ImplicitApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/InterpolationDynamic/bin/Interpolation.dll"
        "$<TARGET_FILE_DIR:ImplicitApp>"
)