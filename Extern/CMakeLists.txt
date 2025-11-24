cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
include(CheckLanguage)

# --- alpaka ---

# alpaka openmp backend
if(RAYX_ENABLE_OPENMP)
    find_package(OpenMP COMPONENTS CXX)
    if(OpenMP_CXX_FOUND)
       set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE ON CACHE BOOL "" FORCE)
       set(alpaka_ACC_CPU_B_SEQ_T_SEQ_ENABLE OFF CACHE BOOL "" FORCE)
    else()
        if(RAYX_REQUIRE_OPENMP)
            message(FATAL_ERROR "No OpenMP CXX compiler found by CMake, but 'RAYX_REQUIRE_OPENMP is set.'")
        else()
            message(WARNING "No OpenMP CXX compiler found by CMake. OpenMP support will be disabled.'")
        endif()

        set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE OFF CACHE BOOL "" FORCE)
        set(alpaka_ACC_CPU_B_SEQ_T_SEQ_ENABLE ON CACHE BOOL "" FORCE)
    endif()
else() # setting could be cached so force it off
    set(alpaka_ACC_CPU_B_OMP2_T_SEQ_ENABLE OFF CACHE BOOL "" FORCE)
    set(alpaka_ACC_CPU_B_SEQ_T_SEQ_ENABLE ON CACHE BOOL "" FORCE)
endif()

# alpaka cuda backend
if(RAYX_ENABLE_CUDA)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
            # This is a set of GPU architechtures, that we compile the device code for
            # A list of relevant GPUs can be found at: https://developer.nvidia.com/cuda-gpus
            set(RAYX_DEFAULT_CUDA_ARCHITECTURES "all-major")

            message(STATUS "Configuration for cuda architectures was not provided. Defaulting to CMAKE_CUDA_ARCHITECTURES=\"${RAYX_DEFAULT_CUDA_ARCHITECTURES}\"")
            set(CMAKE_CUDA_ARCHITECTURES "${RAYX_DEFAULT_CUDA_ARCHITECTURES}" CACHE STRING "Device code gets compiled for specific cuda architectures" FORCE)
        endif()
        message(STATUS "Compiling device code for cuda architectures: \"${CMAKE_CUDA_ARCHITECTURES}\"")

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
else() # setting could be cached so force it off
    set(alpaka_ACC_GPU_CUDA_ENABLE OFF CACHE BOOL "" FORCE)
    set(alpaka_RELOCATABLE_DEVICE_CODE OFF CACHE BOOL "" FORCE)
endif()

set(CMAKE_CUDA_RUNTIME_LIBRARY "Static")
add_subdirectory(alpaka)

# --- HDF5 --- (optional)
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

if (RAYX_BUILD_RAYX_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)

    add_subdirectory(googletest)
endif()

# --- CLI11 ---
if (RAYX_BUILD_RAYX_CLI OR RAYX_BUILD_RAYX_UI)
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
endif()

# --- SDL ---
if(RAYX_BUILD_RAYX_UI)
    # support version in CMakeLists.txt of portable-file-dialogs was removed from cmake. thus we include the header only library by hand
    add_library(portable_file_dialogs INTERFACE)
    target_include_directories(portable_file_dialogs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/portable-file-dialogs)

    add_subdirectory(SDL EXCLUDE_FROM_ALL)
endif()
