cmake_minimum_required(VERSION 3.10)
project(Tests)

find_package(Catch2 REQUIRED)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Define target
add_executable(ai_automata_tests ${TEST_SOURCES} ${CMAKE_SOURCE_DIR}/src/math/NeuralMath.cpp)

# Target properties
target_compile_features(ai_automata_tests PRIVATE cxx_std_17)
target_include_directories(ai_automata_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/../imgui
)

target_link_libraries(ai_automata_tests PRIVATE Catch2::Catch2 Eigen3::Eigen)

add_test(NAME ai_automata_tests COMMAND ai_automata_tests)
