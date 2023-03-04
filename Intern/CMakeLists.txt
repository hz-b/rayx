cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(Intern)
cmake_policy(SET CMP0054 NEW)

# Enable all warnings
if(MSVC)
	message(STATUS "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	message(STATUS "GCC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic") # -Wconversion -Wsign-conversion")
endif()

if(WERROR STREQUAL "YES")
	if(MSVC)
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror") # -O3 finds more errors!
	endif()

	message(STATUS "Werror mode")
endif()

if(VULKAN STREQUAL "NO")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NO_VULKAN")
	message(STATUS "Vulkan disabled")
endif()

if(H5 STREQUAL "NO")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NO_H5")
	message(STATUS "H5 disabled")
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
