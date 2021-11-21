cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 17)

project(RayReworked)

# enable all warnings
if (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Wall")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

if ("$ENV{CI}" STREQUAL "YES")
	# warnings are treated as errors
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror") # -O3 finds more errors!
else()
	# enable vulkan testing
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVULKAN_TEST")
endif()

add_subdirectory(Intern)
add_subdirectory(Extern)
add_subdirectory(Tests)
