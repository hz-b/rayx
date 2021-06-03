cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(RayTests)


include(CTest)
enable_testing()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BINARY ${CMAKE_PROJECT_NAME}_tst)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

#if (WIN32)
    #target_compile_definitions(${BINARY} PUBLIC RAY_PLATFORM_WINDOWS)
#endif ()
target_link_libraries(${BINARY} PUBLIC RayCore gtest gmock)