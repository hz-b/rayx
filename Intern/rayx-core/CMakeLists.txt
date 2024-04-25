cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# ---- for files compiled with cuda compiler, prepend -Xcompiler flags ----
function(prepend_xcompiler FLAG_LIST PREPENDED_FLAG_LIST)
    set(SRC ${${FLAG_LIST}})
    foreach(FLAG ${SRC})
        set(DST ${DST} -Xcompiler="${FLAG}")
    endforeach()
    set(${PREPENDED_FLAG_LIST} ${DST} PARENT_SCOPE)
endfunction()

function(alpaka_target In_Name)
    if(alpaka_ACC_GPU_CUDA_ENABLE)
        enable_language(CUDA)
        # We have to set this here since CUDA_SEPARABLE_COMPILATION is not propagated by the alpaka::alpaka target.
        if(alpaka_RELOCATABLE_DEVICE_CODE STREQUAL ON)
            set_property(TARGET ${In_Name} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
        elseif(alpaka_RELOCATABLE_DEVICE_CODE STREQUAL OFF)
            set_property(TARGET ${In_Name} PROPERTY CUDA_SEPARABLE_COMPILATION OFF)
        endif()
    endif()
endfunction()

function(alpaka_source)
    if(alpaka_ACC_GPU_CUDA_ENABLE)
        enable_language(CUDA)
        foreach(_file ${ARGN})
            if((${_file} MATCHES "\\.cpp$") OR
               (${_file} MATCHES "\\.cxx$") OR
               (${_file} MATCHES "\\.cu$")
            )
                set_source_files_properties(${_file} PROPERTIES LANGUAGE CUDA)
            endif()
        endforeach()
    endif()
endfunction()

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
file(GLOB_RECURSE ALPAKA_SOURCE
    ${PROJECT_SOURCE_DIR}/src/Shader/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Tracer/*.cpp
)
add_library(${PROJECT_NAME} SHARED ${SOURCE})
alpaka_target(${PROJECT_NAME})
alpaka_source(${ALPAKA_SOURCE})

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)

# -----------------

# -----OpenMP--------
find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
    prepend_xcompiler(OpenMP_CXX_FLAGS OpenMP_CUDA_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:${OpenMP_CUDA_FLAGS}>)
else()
    message(STATUS "OpenMP not found")
    target_compile_definitions(${PROJECT_NAME} PUBLIC NO_OMP)
endif()

# -------------------

# ---- Warnings ----
if(MSVC)
    message(STATUS "MSVC")
    set(MSVC_CXX_FLAGS /W3 /wd4251 /wd4267)
    prepend_xcompiler(MSVC_CXX_FLAGS MSVC_CUDA_FLAGS)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MSVC_CXX_FLAGS}>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:${MSVC_CUDA_FLAGS}>)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    message(STATUS "GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra) # -pedantic -Werror)

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

# -------------

# ---- Defines ----
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILE_PLATFORM RAYX_PLATFORM_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILE_PLATFORM RAYX_PLATFORM_MSVC)
else()
    set(COMPILE_PLATFORM RAYX_PLATFORM_UNKNOWN)
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE RAYX_BUILD_DLL
    PUBLIC ${COMPILE_PLATFORM}
    RAYX_PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)

# Only build gpu tracer when CUDA is enabled
if(NOT alpaka_ACC_GPU_CUDA_ENABLE)
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
    ${PROJECT_SOURCE_DIR}/../../Extern/alpaka/include/
)
target_link_libraries(${PROJECT_NAME} PUBLIC alpaka::alpaka)

# ------------------------

# ---- Data ----
# Define the source and destination paths
set(DATA_SRC_DIR "${CMAKE_SOURCE_DIR}/Data")
# Set the destination directory for the Data directory based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DATA_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Data")
else()
    set(DATA_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Data")
endif()

# Copy the Data directory to the binary output directory after building
message(STATUS "Copying Data directory from ${DATA_SRC_DIR} to ${DATA_DST_DIR}")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SRC_DIR} ${DATA_DST_DIR}
    COMMENT "Copying Data directory to build output directory..."
)
# -----------------

# ---- Scripts ----
# Define the source and destination paths
set(SCRIPT_SRC_DIR "${CMAKE_SOURCE_DIR}/Scripts/plot.py")
# Set the destination directory for the Scripts directory based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SCRIPT_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Scripts/plot.py")
else()
    set(SCRIPT_DST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Scripts/plot.py")
endif()

# Copy the Scripts directory to the binary output directory after building
message(STATUS "Copying Scripts directory from ${SCRIPT_SRC_DIR} to ${SCRIPT_DST_DIR}")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_SRC_DIR} ${SCRIPT_DST_DIR}
    COMMENT "Copying Scripts directory to build output directory..."
)
# -----------------

# ---- CPack ----
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Your Project Description")
install(FILES ${SCRIPT_SRC_DIR} DESTINATION bin/Scripts)

include(CPack)

# -----------------
