add_library(PolyLib STATIC src/PolyLib.cpp 
src/Polynomial.cpp
"include/PolyExceptions.h" "include/Polynomial.h" "src/Polynomial.cpp")

target_include_directories(PolyLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(PolyLib PUBLIC cxx_std_20)