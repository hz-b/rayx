cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# ---- Add tests ----
add_subdirectory(tests)
# -------------------

# ---- RayCore ----
project(RayCore)
file(GLOB_RECURSE SOURCE ${RayCore_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE})

# Enable all warnings
if(MSVC)
	message(STATUS "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4) # /WX)
else()
	message(STATUS "GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()


target_precompile_headers(${PROJECT_NAME} PUBLIC raycorePCH.h)
set(CMAKE_PCH_INSTANTIATE_TEMPLATES ON)

# Set Compiler
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
    PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)

if(NOT VULKAN STREQUAL "NO")
    find_package(Vulkan 1.3.216 REQUIRED)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC Vulkan::Vulkan
    )
endif()

# Link H5 Format
if(NOT H5 STREQUAL "NO")
    if(UNIX)
        target_include_directories(${PROJECT_NAME} PUBLIC /usr/include/hdf5/serial)
        target_link_directories(${PROJECT_NAME} PUBLIC /usr/lib/x86_64-linux-gnu/hdf5/serial)
        target_link_libraries(${PROJECT_NAME} PUBLIC hdf5)
    else()
        find_package(HDF5 REQUIRED)
        target_link_libraries(${PROJECT_NAME}
            PUBLIC HDF5::HDF5
        )
    endif()
endif()

# Add Extern deps TODO(Jannis): the src include shouldn't be a SYSTEM include
target_include_directories(${PROJECT_NAME}
    PUBLIC ${RayCore_SOURCE_DIR}/src
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/glm/glm/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/HighFive/include/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/VMA/include/
)

if(NOT VULKAN STREQUAL "NO")
    # Compile Shaders
    # The following code is used to always compile the shader.
    # This is most likely not an optimal solution, but it will work
    # until we find a better one.
    add_dependencies(${PROJECT_NAME} COMPILE_SHADER)
    set(COMPILED_SHADER ${CMAKE_BINARY_DIR}/bin/comp.spv)
    set(COMPILED_SHADER_FAKE ${CMAKE_BINARY_DIR}/bin/___comp.spv) # this exists so file cannot be found -> always execute command

    add_custom_command(
        OUTPUT
        ${COMPILED_SHADER}
        ${COMPILED_SHADER_FAKE}
        COMMAND "glslc"
        ARGS ${RayCore_SOURCE_DIR}/src/Tracer/shader/main-glsl.comp -o ${COMPILED_SHADER} -fshader-stage=compute
    )

    add_custom_target(COMPILE_SHADER ALL DEPENDS ${COMPILED_SHADER})
endif()
