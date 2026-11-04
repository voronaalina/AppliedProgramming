add_executable(PolyTests PolynomialTests.cpp)
target_link_libraries(PolyTests PRIVATE PolyLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(PolyTests)