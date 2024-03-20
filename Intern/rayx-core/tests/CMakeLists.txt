cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Project ----
project(RAY-Core_Tests)
include(CTest)
enable_testing()
set(BINARY rayx-core-tst)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
set(SOURCES ${TEST_SOURCES})
alpaka_add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
# -----------------


# ---- PCH ----
target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
# -------------


# ---- Dependencies ----
target_link_libraries(${BINARY} PUBLIC rayx-core gtest gmock)
# ----------------------
