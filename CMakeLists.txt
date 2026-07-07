cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3")
    endif()
endif()

# ---- Project ----
project(RAYX VERSION 1.1.0)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# ------------------

# ---- Options ----
option(RAYX_WERROR "add -Werror option. Warning will be treated as errors." OFF) # inactive per default
if (RAYX_WERROR)
    message(STATUS "Werror mode is active. Warnings will be treated as errors.")
endif()

option(RAYX_ENABLE_CUDA "This option enables the search for CUDA. Project will be compiled without cuda if not found." ON)
option(RAYX_REQUIRE_CUDA "If option 'RAYX_ENABLE_CUDA' is ON, this option will add the requirement that cuda must be found." OFF)
option(RAYX_ENABLE_OPENMP "This option enables the search for OPENMP. Project will be compiled without openmp if not found." ON)
option(RAYX_REQUIRE_OPENMP "If option 'RAYX_ENABLE_OPENMP' is ON, this option will add the requirement that openmp must be found." OFF)
option(RAYX_ENABLE_H5 "This option enables the search for HDF5. Project will be compiled without HDF5 if not found." ON)
option(RAYX_REQUIRE_H5 "If option 'RAYX_ENABLE_H5' is ON, this option will add the requirement that HDF5 must be found." OFF)
option(RAYX_BUILD_RAYX_CLI "This option builds the RAYX command line interface." ON)
option(RAYX_BUILD_RAYX_UI "This option builds the RAYX graphical user interface." ON)
option(RAYX_BUILD_RAYX_TESTS "This option builds the RAYX test suite." ON)
option(RAYX_STATIC_LIB "This option builds 'rayx-core' as a static library." OFF)
# ------------------

# ---- Specific macos compiler options and flags concerning OpenMP ----
if(APPLE)
    # Detect installed package manager. MacPorts wins if both are present, since
    # its layout (libs directly under /opt/local) does not overlap with Homebrew.
    # CMAKE_SYSTEM_PROCESSOR is only populated after project(), so this lives below it.
    if(EXISTS "/opt/local/bin/port")
        set(MACOS_PKG_PREFIX "/opt/local")
        set(MACOS_PKG_KIND "macports")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(MACOS_PKG_PREFIX "/opt/homebrew")
        set(MACOS_PKG_KIND "homebrew")
    else()
        set(MACOS_PKG_PREFIX "/usr/local")
        set(MACOS_PKG_KIND "homebrew")
    endif()
    message(STATUS "macOS package manager: ${MACOS_PKG_KIND} at ${MACOS_PKG_PREFIX}")
    set(CMAKE_INSTALL_RPATH "@loader_path;@rpath;${MACOS_PKG_PREFIX}/lib")

    # OpenMP: MacPorts ships libomp directly under /opt/local; Homebrew uses an
    # opt/<formula> subdirectory.
    if(MACOS_PKG_KIND STREQUAL "macports")
        set(OpenMP_INCLUDE_DIR "${MACOS_PKG_PREFIX}/include/libomp")
        set(OpenMP_LIBRARY "${MACOS_PKG_PREFIX}/lib/libomp.dylib")
    else()
        set(OpenMP_INCLUDE_DIR "${MACOS_PKG_PREFIX}/opt/libomp/include")
        set(OpenMP_LIBRARY "${MACOS_PKG_PREFIX}/opt/libomp/lib/libomp.dylib")
    endif()

    # Hints for FindOpenMP. The system AppleClang/Homebrew clang needs the
    # -Xpreprocessor invocation and an explicit libomp path. These are only
    # hints: the actual find_package(OpenMP) call lives in Extern/CMakeLists.txt,
    # where it is gated by RAYX_ENABLE_OPENMP / RAYX_REQUIRE_OPENMP and linked
    # via the OpenMP::OpenMP_CXX target. We deliberately do not call
    # find_package here, nor add global compile/link flags, to keep the option
    # semantics intact and avoid linking libomp twice.
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${OpenMP_INCLUDE_DIR}")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${OpenMP_INCLUDE_DIR}")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${OpenMP_LIBRARY}")
    set(OpenMP_omp_INCLUDE_DIRS "${OpenMP_INCLUDE_DIR}")

    # HDF5 hint: MacPorts installs into /opt/local directly; Homebrew uses
    # opt/hdf5. The actual find_package(HDF5) is done in Extern/CMakeLists.txt
    # (gated by RAYX_ENABLE_H5 / RAYX_REQUIRE_H5); HDF5_ROOT is inherited there.
    if(MACOS_PKG_KIND STREQUAL "macports")
        set(HDF5_ROOT "${MACOS_PKG_PREFIX}")
    else()
        set(HDF5_ROOT "${MACOS_PKG_PREFIX}/opt/hdf5")
    endif()
endif()


# ---- Build options ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/debug)
# ------------------


# ---- RAYX directory ----

set(RAYX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ------------------------

# ---- Code Coverage ----
set(CMAKE_MODULE_PATH ${RAYX_SOURCE_DIR}/Extern/cmake)
# ------------------

# ---- CPack ----
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_NAME} - For simulating and designing beamlines at synchrotron light sources")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - For simulating and designing beamlines at synchrotron light sources")
set(CPACK_PACKAGE_VENDOR "Helmhotz-Zentrum Berlin")
set(CPACK_PACKAGE_CONTACT "jannis.maier@helmholtz-berlin.de")

# Disable GoogleTest installation
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of GoogleTest" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "Disable installation of GoogleMock" FORCE)

# Install directories
if(APPLE)
    set(INSTALL_DATA_DIR "Library/Application Support/${PROJECT_NAME}")
    set(INSTALL_FONTS_DIR "Library/Fonts/${PROJECT_NAME}")
elseif(UNIX AND NOT APPLE)
    set(INSTALL_DATA_DIR "share/${PROJECT_NAME}")
    set(INSTALL_FONTS_DIR "share/fonts/${PROJECT_NAME}")
elseif(WIN32)
    set(INSTALL_DATA_DIR ".")
    set(INSTALL_FONTS_DIR ".")
endif()

if(RAYX_CUSTOM_DATA_DIR)
    set(INSTALL_DATA_DIR ${RAYX_CUSTOM_DATA_DIR})
endif()
# ------------------


# ---- Subdirectories ----
add_subdirectory(Extern)
add_subdirectory(Intern)
# ------------------
