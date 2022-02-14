cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(TerminalApp)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)


#Add Python files
file(COPY python DESTINATION ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} main.cpp TerminalApp.cpp TerminalApp.h PythonInterp.cpp)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RAYX_PLATFORM_WINDOWS)
endif ()

find_package(Python3 REQUIRED)
find_package (PythonLibs REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/src)

include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} RayCore)

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})