cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Project ----
project(RAYX VERSION 0.21.7)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# ------------------

# ---- Options ----
option(WERROR "add -Werror option" "NO") # inactive per default
option(RAYX_ENABLE_CUDA "This option enables the search for CUDA. Project will be compiled without cuda if not found." ON)
option(RAYX_REQUIRE_CUDA "If option 'RAYX_ENABLE_CUDA' is ON, this option will add the requirement that cuda must be found." OFF)
option(RAYX_ENABLE_OPENMP "This option enables the search for OPENMP. Project will be compiled without openmp if not found." ON)
option(RAYX_REQUIRE_OPENMP "If option 'RAYX_ENABLE_OPENMP' is ON, this option will add the requirement that openmp must be found." OFF)
option(RAYX_STATIC_LIB "This option builds 'rayx-core' as a static library." OFF)
# ------------------

# ---- Specific macos compiler options and flags concerning OpenMP ----
if(APPLE)
    # Detect architecture and package manager
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        set(HOMEBREW_PREFIX "/opt/homebrew")
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
    
    # MacPorts detection
    if(EXISTS "/opt/local/include/libomp")
        set(OpenMP_INCLUDE_DIR "/opt/local/include/libomp")
        set(OpenMP_LIBRARY "/opt/local/lib/libomp.dylib")
        message(STATUS "Using MacPorts OpenMP installation")
    else()
        # Homebrew fallback
        set(OpenMP_INCLUDE_DIR "${HOMEBREW_PREFIX}/opt/libomp/include")
        set(OpenMP_LIBRARY "${HOMEBREW_PREFIX}/opt/libomp/lib/libomp.dylib")
        message(STATUS "Using Homebrew OpenMP installation")
    endif()

    # Common OpenMP settings
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${OpenMP_INCLUDE_DIR}")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${OpenMP_INCLUDE_DIR}")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${OpenMP_LIBRARY}")
    set(OpenMP_omp_INCLUDE_DIRS "${OpenMP_INCLUDE_DIR}")
    
    if(RAYX_ENABLE_OPENMP)
        find_package(OpenMP REQUIRED)
        if(OpenMP_CXX_FOUND)
            message(STATUS "OpenMP found:")
            message(STATUS "  Include dirs: ${OpenMP_CXX_INCLUDE_DIRS}")
            message(STATUS "  Libraries: ${OpenMP_CXX_LIBRARIES}")
        
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
            include_directories(${OpenMP_omp_INCLUDE_DIRS})
            link_libraries(${OpenMP_omp_LIBRARY})
        endif()
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
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Extern/cmake)
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
# ------------------


# ---- Subdirectories ----
add_subdirectory(Extern)
add_subdirectory(Intern)
# ------------------
