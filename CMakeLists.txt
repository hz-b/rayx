cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

project(RAYX)

# enable all warnings
if(MSVC)
	message(STATUS "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	message(STATUS "GCC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic") # -Wconversion -Wsign-conversion")
endif()

if("$ENV{CI}" STREQUAL "YES")
	# warnings are treated as errors
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -D CI") # -O3 finds more errors!
	message(STATUS "CI mode")
endif()

# -------CodeCoverage-------
option(BUILD_WITH_GCOV "Add Code Coverage to build" FALSE)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Extern/cmake)
#set(BUILD_WITH_GCOV TRUE)
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_WITH_GCOV)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
	include(CodeCoverage)
	APPEND_COVERAGE_COMPILER_FLAGS()
	set(CODE_COVERAGE_VERBOSE FALSE)
	setup_target_for_coverage_lcov(NAME ${PROJECT_NAME}_coverage_lcov
		EXECUTABLE ${CMAKE_PROJECT_NAME}_tst
	)

	# For gcovr
	# setup_target_for_coverage_gcovr_html(NAME ${PROJECT_NAME}_coverage_gcovr_html
	# EXECUTABLE ${CMAKE_PROJECT_NAME}_tst)
endif()

# ---------------------------
add_subdirectory(Intern)
add_subdirectory(Extern)
add_subdirectory(Tests)
