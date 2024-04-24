cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
include(CheckLanguage)

set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)

add_subdirectory(googletest)
add_subdirectory(NFD-Extended)

# add_subdirectory(glfw)

# CLI11
set(CLI11_SANITIZERS OFF)
set(CLI11_BUILD_DOCS OFF)
set(CLI11_BUILD_TESTS OFF)
set(CLI11_BUILD_EXAMPLES OFF)
set(CLI11_BUILD_EXAMPLES_JSON OFF)
set(CLI11_SINGLE_FILE_TESTS OFF)
set(CLI11_INSTALL OFF)
set(CLI11_FORCE_LIBCXX OFF)
set(CLI11_CUDA_TESTS OFF)
set(CLI11_CLANG_TIDY OFF)
add_subdirectory(CLI11)

# -------------------

# ---- Check for CUDA and configure Alpaka ----
set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE OFF CACHE BOOL "")
set(alpaka_ACC_CPU_B_SEQ_T_OMP2_ENABLE OFF CACHE BOOL "")
set(alpaka_ACC_CPU_B_TBB_T_SEQ_ENABLE OFF CACHE BOOL "")
set(alpaka_ACC_CPU_B_SEQ_T_THREADS_ENABLE OFF CACHE BOOL "")
set(alpaka_ACC_CPU_B_SEQ_T_SEQ_ENABLE ON CACHE BOOL "")

check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
    set(alpaka_ACC_GPU_CUDA_ENABLE ON CACHE BOOL "")
    set(alpaka_RELOCATABLE_DEVICE_CODE ON CACHE BOOL "")
else()
    message(WARNING "Cuda compiler was not found by CMake. GPU tracer will be disabled.")
    set(NO_CUDA ON)
endif()

add_subdirectory(alpaka)
