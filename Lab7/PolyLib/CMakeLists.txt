add_library(PolyLib STATIC src/PolyLib.cpp)

target_include_directories(PolyLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(PolyLib PUBLIC cxx_std_20)