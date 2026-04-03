cmake_minimum_required(VERSION 3.10)
project(Tests)

find_package(Catch2 REQUIRED)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Define target
# Include all source files: tests + mocks + core implementations
add_executable(ai_automata_tests
    ${TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/math/NeuralMath.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ca/CellularAutomaton.cpp
    ${CMAKE_SOURCE_DIR}/src/core/anns/NeuralNetwork.cpp
)

# Target properties
target_compile_features(ai_automata_tests PRIVATE cxx_std_17)
target_include_directories(ai_automata_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/test
    ${CMAKE_SOURCE_DIR}/../imgui
)

target_link_libraries(ai_automata_tests PRIVATE Catch2::Catch2 Eigen3::Eigen)

add_test(NAME ai_automata_tests COMMAND ai_automata_tests)
