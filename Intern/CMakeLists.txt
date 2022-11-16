cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(Intern)
cmake_policy(SET CMP0054 NEW)

# Enable all warnings
if(MSVC)
	message(STATUS "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	message(STATUS "GCC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lmgl -Wall -Wextra -Wpedantic") # -Wconversion -Wsign-conversion")
endif()

# Warnings are treated as errors on the CI Pipeline
if("$ENV{CI}" STREQUAL "YES")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror -D CI") # -O3 finds more errors!
	message(STATUS "CI mode")
endif()

# Release repo
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)

# Debug repo
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)


add_subdirectory(RayCore)
add_subdirectory(TerminalApp)
