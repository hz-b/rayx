cmake_minimum_required(VERSION 3.2 FATAL_ERROR)


# ---- Project ----
project(RAYX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -----------------


# ---- Subdirectories ----
add_subdirectory(Intern)
add_subdirectory(Extern)
# ------------------------


# ---- Options ----
option(WERROR "add -Werror option" "NO") # inactive per default
option(VULKAN "include the VulkanTracer" "YES") # activated per default
option(H5 "include the H5Writer" "YES") # active per default
# ------------------


# ---- Code Coverage ----
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
# -----------------------
