cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Project ----
project(RayTests)
include(CTest)
enable_testing()
set(BINARY ${CMAKE_PROJECT_NAME}_tst)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
# -----------------


# ---- PCH ----
target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
# -------------


# ---- Dependencies ----
target_link_libraries(${BINARY} PUBLIC RayCore gtest gmock)
# ----------------------


# ---- Code Coverage ----
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_WITH_GCOV)
    FILE(GLOB_RECURSE GCNO_FILES ${CMAKE_BINARY_DIR}/*/*.gcno)
    FILE(GLOB_RECURSE GCDA_FILES ${CMAKE_BINARY_DIR}/*/*.gcda)
    set_property(TARGET ${BINARY}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${TARGET} ${GCNO_FILES} ${GCDA_FILES})
endif()
# -----------------------