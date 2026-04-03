cmake_minimum_required(VERSION 3.10)
project(ai_automata VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
if(NOT DEFINED IMGUI_DIR)
    set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/../imgui")
endif()
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")

# ImGui Core
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Application Sources
file(GLOB_RECURSE APP_SOURCES
    "${SRC_DIR}/*.cpp"
)

# Dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

# Pipeline Options (Sanitizer & Coverage, ImGui Ini)
option(ENABLE_SANITIZER "Build with Address and Undefined Behavior Sanitizers" OFF)
option(ENABLE_COVERAGE "Build with Coverage for gcov/lcov" OFF)

if(ENABLE_SANITIZER)
    message(STATUS "Enabling Address and UB Sanitizers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()

if(ENABLE_COVERAGE)
    message(STATUS "Enabling Coverage Generation")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Pass ImGui INI path to code so it resolves to project root
add_compile_definitions(IMGUI_INI_PATH="${CMAKE_SOURCE_DIR}/imgui.ini")

# Executable
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${IMGUI_SOURCES})

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen glfw OpenGL::GL)

# Install Application
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Doxygen Support
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # create Doxyfile manually or through cmake, but assuming it's managed separately
endif()

# Tests
enable_testing()
add_subdirectory(test)
