cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(TerminalApp)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)

add_executable(${PROJECT_NAME} main.cpp TerminalApp.cpp TerminalApp.h)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RAYX_PLATFORM_WINDOWS)
endif ()


target_include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} RayCore)