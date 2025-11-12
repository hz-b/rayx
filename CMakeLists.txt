cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

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
option(RAYX_BUILD_TESTS "This option builds the RAYX test suite." ON)
option(RAYX_BUILD_RAYX_CLI "This option builds the RAYX command line interface." ON)
option(RAYX_BUILD_RAYX_UI "This option builds the RAYX graphical user interface." ON)
option(RAYX_STATIC_LIB "This option builds 'rayx-core' as a static library." OFF)
# ------------------


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
