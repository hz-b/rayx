cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(RayReworked)

add_subdirectory(Tracer)
add_subdirectory(RayCore)
add_subdirectory(TerminalApp)
