cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# Define output directories for project
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

cmake_policy(SET CMP0054 NEW)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DRAYCORE_DEBUG_MODE")

# Set Debug flag so debug macro in Debug.h works
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )
# set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC" )

project(Intern)

add_subdirectory(Tracer)
add_subdirectory(RayCore)
add_subdirectory(TerminalApp)
