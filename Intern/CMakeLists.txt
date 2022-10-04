cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

cmake_policy(SET CMP0054 NEW)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DRAY_DEBUG_MODE")
# TODO(Jannis): add -fPIC for linux

# if (NOT "$ENV{CI}" STREQUAL "YES") # lhdf5 doesn't exist in CI
# 	# for ubuntu users:
# 	include_directories("/usr/include/hdf5/serial")
# 	link_directories("/usr/lib/x86_64-linux-gnu/hdf5/serial")
# 	link_libraries("hdf5")
# endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug/lib)

project(Intern)

add_subdirectory(RayCore)
add_subdirectory(TerminalApp)
