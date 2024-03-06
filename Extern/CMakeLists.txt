cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

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

# set(Kokkos_ENABLE_EXAMPLES OFF)
# set(Kokkos_ENABLE_SERIAL ON)
# set(Kokkos_ENABLE_THREADS OFF)
# set(Kokkos_ENABLE_OPENMP OFF)
# set(Kokkos_ENABLE_CUDA OFF)
# TODO: this hack enables position independent code (fPIC) for kokkos. can we do better?
#set(BUILD_SHARED_LIBS ON)
add_subdirectory(kokkos)
