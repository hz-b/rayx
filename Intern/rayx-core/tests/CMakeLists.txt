cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Project ----
project(RAY-Core_Tests)
include(CTest)
enable_testing()
set(BINARY rayx-core-tst)
file(GLOB_RECURSE SOURCE *.h *.cpp)
add_executable(${BINARY} ${SOURCE})
add_test(NAME ${BINARY} COMMAND ${BINARY})
# -----------------


# ---- PCH ----
target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
# -------------


# ---- Dependencies ----
target_link_libraries(${BINARY} PUBLIC rayx-core gtest gmock)
# ----------------------
