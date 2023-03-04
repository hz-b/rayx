cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(RayTests)

# if (NOT "$ENV{CI}" STREQUAL "YES") # lhdf5 doesn't exist in CI
# # for ubuntu users:
# include_directories("/usr/include/hdf5/serial")
# link_directories("/usr/lib/x86_64-linux-gnu/hdf5/serial")

# link_libraries("hdf5")
# endif()
include(CTest)
enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)

set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# if (WIN32)
# target_compile_definitions(${BINARY} PUBLIC RAYX_PLATFORM_MSVC)
# endif ()
target_link_libraries(${BINARY} PUBLIC RayCore gtest gmock)

#Clean Coverage Files
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_WITH_GCOV)
    FILE(GLOB_RECURSE GCNO_FILES ${CMAKE_BINARY_DIR}/*/*.gcno)
    FILE(GLOB_RECURSE GCDA_FILES ${CMAKE_BINARY_DIR}/*/*.gcda)
    set_property(TARGET ${BINARY}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET} ${GCNO_FILES} ${GCDA_FILES})
endif()


# TODO combine this with the Intern/CMakeFiles.txt, it does the same.
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
