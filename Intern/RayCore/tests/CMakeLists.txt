cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(RayTests)

include(CTest)
enable_testing()

set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

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
