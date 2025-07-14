cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
include(CheckLanguage)

set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)

add_subdirectory(googletest)

# support version in CMakeLists.txt of portable-file-dialogs was removed from cmake. thus we include the header only library by hand
add_library(portable_file_dialogs INTERFACE)
target_include_directories(portable_file_dialogs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/portable-file-dialogs)

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

# alpaka
# alpaka openmp backend (optional
if(RAYX_ENABLE_OPENMP)
    find_package(OpenMP COMPONENTS CXX)
    if(OpenMP_CXX_FOUND)
       set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE ON CACHE BOOL "" FORCE)
    else()
        if(RAYX_REQUIRE_OPENMP)
            message(FATAL_ERROR "No OpenMP CXX compiler found by CMake, but 'RAYX_REQUIRE_OPENMP is set.'")
        else()
            message(WARNING "No OpenMP CXX compiler found by CMake. OpenMP support will be disabled.'")
        endif()
    endif()
else() # setting could be cached so force it off
    set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE OFF CACHE BOOL "" FORCE)
endif()

# alpaka cuda backend (optional
if(RAYX_ENABLE_CUDA)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        set(alpaka_ACC_GPU_CUDA_ENABLE ON CACHE BOOL "" FORCE)
        set(alpaka_RELOCATABLE_DEVICE_CODE ON CACHE BOOL "" FORCE)
        set(alpaka_DISABLE_VENDOR_RNG ON CACHE BOOL "" FORCE)
    else()
        if(RAYX_REQUIRE_CUDA)
            message(FATAL_ERROR "Cuda compiler was not found by CMake. Option 'RAYX_REQUIRE_CUDA' is set. Terminating.")
        else()
            message(WARNING "Cuda compiler was not found by CMake. GPU tracer will be disabled.")
        endif()
    endif()
else()
    set(alpaka_ACC_GPU_CUDA_ENABLE OFF CACHE BOOL "" FORCE)
    set(alpaka_RELOCATABLE_DEVICE_CODE OFF CACHE BOOL "" FORCE)
endif()

if(NOT alpaka_ACC_GPU_CUDA_ENABLE AND NOT alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE)
    message(FATAL_ERROR "Neither OpenMP or Cuda backend have been enabled. At least one is required. Aborting.")
endif()

set(CMAKE_CUDA_RUNTIME_LIBRARY "Static")
add_subdirectory(alpaka)

# HDF5 (optional)
if(RAYX_ENABLE_H5)
    find_package(HDF5 GLOBAL REQUIRED COMPONENTS C)
    if(HDF5_FOUND)
        set(RAYX_H5_ENABLED ON PARENT_SCOPE)
        set(RAYX_HDF5_LIBRARIES ${HDF5_LIBRARIES} PARENT_SCOPE)
        set(RAYX_HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS} PARENT_SCOPE)
        set(RAYX_HDF5_DEFINITIONS ${HDF5_DEFINITIONS} PARENT_SCOPE)

        set(HIGHFIVE_FIND_HDF5 OFF CACHE BOOL "" FORCE)
        add_subdirectory(highfive)
    else()
        if(RAYX_REQUIRE_H5)
            message(FATAL_ERROR "Library 'HDF5' was not found CMake, but option 'RAYX_REQUIRE_H5' is set. Terminating.")
        else()
            message(WARNING "Library 'HDF5' was not found by CMake. H5 file format will be disabled.")
        endif()
    endif()
endif()

# SDL
add_subdirectory(SDL EXCLUDE_FROM_ALL)
