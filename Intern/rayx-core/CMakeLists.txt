cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- Code Coverage ----
option(BUILD_WITH_COV "Add Code Coverage to build" TRUE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND BUILD_WITH_COV)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    message("Building with llvm-cov support")
endif()

# -----------------------

# ---- Add tests ----
add_subdirectory(tests)

# -------------------

# ---- Project ----
project(rayx-core VERSION ${RAYX_VERSION})
file(GLOB_RECURSE SOURCE ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

# -----------------


# -----OpenMP--------
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
else()
    message(STATUS "OpenMP not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_OMP)
endif()

# -------------------

# ---- Warnings ----
if(MSVC)
    message(STATUS "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /wd4251 /wd4267)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    message(STATUS "GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic) # -Werror)

    # Enable werror if requested (mostly used for CI)
    if(WERROR STREQUAL "YES")
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Werror)
        message(STATUS "Werror mode")
    endif()
endif()

# ------------------

# ---- Disable H5 ----

find_package(HDF5)

if(HDF5_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${HDF5_LIBRARIES})
    target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${HDF5_INCLUDE_DIRS})
else()
    message(STATUS "HDF5 not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_H5)
endif()

# -------------------------

# ---- PCH ----
target_precompile_headers(${PROJECT_NAME} PUBLIC raycorePCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# -------------

# ---- Defines ----
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_PLATFORM RAYX_PLATFORM_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAYX_PLATFORM_MSVC)
else()
    set(COMPILE_PLATFORM UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RAYX_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
    RAYX_PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)

# Only build gpu tracer when CUDA is enabled
if(NOT ${Kokkos_ENABLE_CUDA})
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_GPU_TRACER)
endif()

# -----------------

# ---- Dependencies ----
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    ${PROJECT_SOURCE_DIR}/../../Extern/glm/glm/
    ${PROJECT_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    ${PROJECT_SOURCE_DIR}/../../Extern/HighFive/include/
    ${PROJECT_SOURCE_DIR}/../../Extern/VMA/include/
    ${Kokkos_INCLUDE_DIRS_RET}
)
target_link_libraries(${PROJECT_NAME} PUBLIC Kokkos::kokkos)

# ------------------------

# ---- CPack ----

include(InstallRequiredSystemLibraries)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../../LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Your Project Description")

include(CPack)

# -----------------
