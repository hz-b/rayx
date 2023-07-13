cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(RayCore)

SET(SOURCE
    src/CanonicalizePath.cpp
    src/Random.cpp
    src/utils.cpp
    src/Data/Importer.cpp
    src/Data/xml.cpp
    src/Data/DatFile.cpp
    src/Debug/Debug.cpp
    src/Debug/Instrumentor.cpp
    src/Beamline/Beamline.cpp
    src/Beamline/OpticalElement.cpp
    src/Beamline/LightSource.cpp
    src/Beamline/EnergyDistribution.cpp
    src/Beamline/Objects/ImagePlane.cpp
    src/Beamline/Objects/MatrixSource.cpp
    src/Beamline/Objects/PointSource.cpp
    src/Beamline/Objects/Ellipsoid.cpp
    src/Beamline/Objects/ReflectionZonePlate.cpp
    src/Beamline/Objects/Slit.cpp
    src/Beamline/Objects/SphereGrating.cpp
    src/Beamline/Objects/PlaneGrating.cpp
    src/Beamline/Objects/PlaneMirror.cpp
    src/Beamline/Objects/PointSource.cpp
    src/Beamline/Objects/SphereMirror.cpp
    src/Beamline/Objects/ToroidMirror.cpp
    src/Beamline/Objects/Cylinder.cpp
    src/Beamline/Objects/Cone.cpp
    src/Material/Material.cpp
    src/Material/NffTable.cpp
    src/Material/PalikTable.cpp
    src/Tracer/Tracer.cpp
    src/Tracer/VulkanTracer.cpp
    src/Tracer/CpuTracer.cpp
    src/VulkanEngine/Cleanup.cpp
    src/VulkanEngine/Buffer/PushConstant.cpp
    src/VulkanEngine/Destruct.cpp
    src/VulkanEngine/Init/CommandPool.cpp
    src/VulkanEngine/Init/Instance.cpp
    src/VulkanEngine/Init/Init.cpp
    src/VulkanEngine/Init/PickDevice.cpp
    src/VulkanEngine/Init/CommandBuffer.cpp
    src/VulkanEngine/Init/Fence.cpp
    src/VulkanEngine/Init/ShaderStage.cpp
    src/VulkanEngine/Init/Descriptor.cpp
    src/VulkanEngine/Init/Initializers.cpp
    src/VulkanEngine/Run/RecordBuffer.cpp
    src/VulkanEngine/Run/RunCommandBuffer.cpp
    src/VulkanEngine/Buffer/VulkanBuffer.cpp
    src/VulkanEngine/Buffer/BufferHandler.cpp
    src/VulkanEngine/Run/Pipeline.cpp
    src/VulkanEngine/Run/Run.cpp
    src/Writer/CSVWriter.cpp
    src/Writer/H5Writer.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE})

target_precompile_headers(${PROJECT_NAME} PRIVATE raycorePCH.h)
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
    SYSTEM PUBLIC ${RayCore_SOURCE_DIR}/src
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/glm/glm/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/rapidxml-1.13/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/HighFive/include/
    PUBLIC ${RayCore_SOURCE_DIR}/../../Extern/VMA/include/
)
# Compile Shaders
set(SHADER_SOURCES_DIR ${RayCore_SOURCE_DIR}/src/Tracer/shader)

set(SHADER_SOURCES
    singleBounce-glsl.comp)

if(NOT VULKAN STREQUAL "NO")
    # The following code is used to always compile the shaders.
    # This is most likely not an optimal solution, but it will work
    # until we find a better one.
    add_dependencies(${PROJECT_NAME} COMPILE_SHADER)
    set(COMPILED_SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
    set(COMPILED_SHADERS_LIST "") 

    if(WIN32)
        set(SHADER_COMPILER_COMMAND "$ENV{VULKAN_SDK}/Bin/glslc.exe")
    elseif(UNIX)
        set(SHADER_COMPILER_COMMAND "glslc")
    endif()
    
    foreach(SHADER_SOURCE ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)
        # Remove "-glsl" suffix using regular expression replacement
        string(REGEX REPLACE "-glsl$" "" SHADER_NAME ${SHADER_NAME})
        set(COMPILED_SHADER_OUTPUT_FILE ${COMPILED_SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv)
        set(COMPILED_SHADER_FAKE ${COMPILED_SHADER_OUTPUT_DIR}/___${SHADER_NAME}.spv)

        add_custom_command(
            OUTPUT
            ${COMPILED_SHADER_OUTPUT_FILE}
            ${COMPILED_SHADER_FAKE}
            COMMAND ${SHADER_COMPILER_COMMAND}
            ARGS ${SHADER_SOURCES_DIR}/${SHADER_SOURCE} -o ${COMPILED_SHADER_OUTPUT_FILE} -fshader-stage=compute
            COMMENT "Compiling shader: ${SHADER_NAME}"
        )

        list(APPEND COMPILED_SHADERS_LIST ${COMPILED_SHADER_OUTPUT_FILE})
    endforeach()

    add_custom_target(COMPILE_SHADER ALL DEPENDS ${COMPILED_SHADERS_LIST})
endif()

# Print include directories
get_target_property(INCLUDE_DIRECTORIES ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message(STATUS "Include directories for ${PROJECT_NAME}: ${INCLUDE_DIRECTORIES}")
