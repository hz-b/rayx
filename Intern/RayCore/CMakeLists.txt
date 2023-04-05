cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# ---- Add tests ----
add_subdirectory(tests)
# -------------------


# ---- Project ----
project(RayCore)
file(GLOB_RECURSE SOURCE ${RayCore_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE})
# -----------------


# ---- Warnings ----
if(MSVC)
	message(STATUS "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4) # /WX)
else()
	message(STATUS "GCC")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic) #-Werror)

    # Enable werror if requested (mostly used for CI)
    if(WERROR STREQUAL "YES")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror") # -O3 finds more errors!
        message(STATUS "Werror mode")
    endif()
endif()
# ------------------


# ---- Disable Vulkan/H5 ----
if(VULKAN STREQUAL "NO")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NO_VULKAN")
    message(STATUS "Vulkan disabled")
else()
    find_package(Vulkan 1.3.216 REQUIRED)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC Vulkan::Vulkan
    )
endif()
if(H5 STREQUAL "NO")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NO_H5")
    message(STATUS "H5 disabled")
else()
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
    PROJECT_DIR="${CMAKE_SOURCE_DIR}"
    $<$<CONFIG:Debug>:RAYX_DEBUG_MODE>
    $<$<CONFIG:RelWithDebInfo>:RAYX_DEBUG_MODE>
)
# -----------------


# ---- Dependencies ----
target_include_directories(${PROJECT_NAME} PRIVATE ${RayCore_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM INTERFACE ${RayCore_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    ${RayCore_SOURCE_DIR}/../../Extern/glm/glm/
    ${RayCore_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    ${RayCore_SOURCE_DIR}/../../Extern/HighFive/include/
    ${RayCore_SOURCE_DIR}/../../Extern/VMA/include/
)
# ----------------------


# ---- Compile Shaders ----
if(NOT VULKAN STREQUAL "NO")
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
# ------------------------
