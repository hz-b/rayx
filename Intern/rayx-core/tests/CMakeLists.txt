cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Project ----
project(RAY-Core_Tests)
include(CTest)
enable_testing()
set(BINARY rayx-core-tst)
file(GLOB_RECURSE SOURCE *.h *.cpp)
add_executable(${BINARY} ${SOURCE})
set_target_properties(${BINARY} PROPERTIES
    BUILD_RPATH "$ORIGIN;$ORIGIN/lib;$ORIGIN/../lib" # also $ORIGIN/../../lib/$<CONFIG> will be added automatically by CMake for multi-config generators
    INSTALL_RPATH "$ORIGIN;$ORIGIN/lib;$ORIGIN/../lib" # TODO: review if this is necessary
)
add_test(NAME ${BINARY} COMMAND ${BINARY})
# -----------------


# ---- PCH ----
target_precompile_headers(${BINARY} PRIVATE testsPCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)
# -------------


# ---- Dependencies ----
target_link_libraries(${BINARY} PUBLIC rayx-core gtest gmock)
# ----------------------
